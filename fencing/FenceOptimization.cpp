#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/AtomicOrdering.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdint>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "FencingPasses.h"

using namespace llvm;

AtomicOrdering getOrdering(Instruction *inst) {
  if (isa<LoadInst>(inst)) {
    return cast<LoadInst>(inst)->getOrdering();
  } else if (isa<StoreInst>(inst)) {
    return cast<StoreInst>(inst)->getOrdering();
  } else if (isa<FenceInst>(inst)) {
    return cast<FenceInst>(inst)->getOrdering();
  }
  return AtomicOrdering::NotAtomic;
}

class Node {
public:
  BasicBlock *BB;
  Instruction *lastMemOp = nullptr;
  AtomicOrdering order = AtomicOrdering::NotAtomic;
  int name = 0;
  // Tells you if the node is after the last memory operation, place a fence
  // accordingly
  bool after = false;
  std::vector<Node *> successors;
  std::vector<Node *> predecessors;
  Node(BasicBlock *bb) : BB(bb), name(0) {} // Can I have default constructor?
  Node(BasicBlock *bb, Instruction *lastMemOp, AtomicOrdering order)
      : BB(bb), lastMemOp(lastMemOp), order(order), name(0) {}
  Node(BasicBlock *bb, Instruction *lastMemOp, AtomicOrdering order, bool after)
      : BB(bb), lastMemOp(lastMemOp), order(order), after(after), name(0) {}

  Node getNodeAfter(Instruction *inst) {
    Node node = Node(inst->getParent(), inst, getOrdering(inst), true);
    return node;
  }

  Node getNodeBefore(Instruction *inst) {
    Node node = Node(inst->getParent(), inst, getOrdering(inst), false);
    return node;
  }

  bool operator==(const Node &other) const {
    return BB == other.BB && lastMemOp == other.lastMemOp &&
           order == other.order && after == other.after && name == other.name;
  }

  bool operator!=(const Node &other) const { return !(*this == other); }
};

struct Edge {
  uint64_t src;
  uint64_t dst;

  uint32_t capacity;
  bool residual;

  Edge *reverse;

  Edge(uint64_t src, uint64_t dst, uint32_t capacity, bool residual,
       Edge *reverse)
      : src(src), dst(dst), capacity(capacity),
        residual(residual), reverse(reverse) {};

  Edge(uint64_t src, uint64_t dst, uint32_t capacity, bool residual)
      : src(src), dst(dst), capacity(capacity),
        residual(residual), reverse(nullptr) {};

                std::ostream
            & operator<<(std::ostream &os) {
    os << "{src: " << src << ", dst: " << dst << ", capacity: " << capacity
       << ", residual: " << residual << "}";
    return os;
  }
};


using AList_t = std::map<uint64_t, std::vector<Edge*>>;

struct Graph {
  Node *source;
  Node *sink;

  std::vector<Node *> nodes;
  std::vector<std::pair<uint64_t, uint64_t>> edges;

  void addNode(Node *node) {
    for (auto existingNode : nodes) {
      if (*existingNode == *node) {
        return;
      }
    }
    nodes.push_back(node);
  }

  Node *getNode(uint64_t idx) {
    if (idx >= nodes.size())
      return nullptr;

    auto &node = nodes[idx];
    return node;
  }

  void addEdge(Node *from, Node *to) {
    int fromIndex = -1;
    int toIndex = -1;

    for (size_t i = 0; i < nodes.size(); ++i) {
      if (*nodes[i] == *from) {
        fromIndex = i;
      }
      if (*nodes[i] == *to) {
        toIndex = i;
      }
    }

    if (fromIndex == -1 || toIndex == -1) {
      llvm::errs() << "Error: Node not found in graph.\n";
      return;
    }

    // Check if the edge already exists
    for (const auto &edge : edges) {
      if (edge.first == fromIndex && edge.second == toIndex) {
        return; // Edge already exists
      }
    }
    edges.push_back(std::make_pair(fromIndex, toIndex));
  }

  AList_t buildAdjacencyList(uint32_t Capacity) {
    AList_t AdjacencyList;

    for (auto &[u, v] : edges) {
      uint32_t c;
      if (u == 0 || v == 1) // Sink or source edges
        c = 0xFFFFFFFF;
      else
        c = Capacity;

      auto e = new Edge(u, v, c, false);
      auto re = new Edge(v, u, 0, true, e);
      e->reverse = re;

      AdjacencyList[u].push_back(e);
      AdjacencyList[v].push_back(re);
    }

    return AdjacencyList;
  }
};

Node getNodeAtBeginning(BasicBlock *bb) {
  Instruction *firstInst = &*bb->begin();
  Node node = Node(bb, firstInst, getOrdering(firstInst), false);
  node.after = false;
  return node;
}

Node getNodeAtEnd(BasicBlock *bb) {
  Instruction *lastInst = &*bb->rbegin();
  Node node = Node(bb, lastInst, getOrdering(lastInst), true);
  node.after = true;
  return node;
}

Instruction *getLastInst(BasicBlock *bb) {
  Instruction *lastInst = &*bb->rbegin();
  return lastInst;
}

Instruction *getFirstInst(BasicBlock *bb) {
  Instruction *firstInst = &*bb->begin();
  return firstInst;
}

void augment(std::vector<Edge*> Path) {
  uint32_t bottleneck = 0xFFFFFFFF;

  for (auto &E : Path) {
    bottleneck = std::min(E->capacity, bottleneck);
  }

  llvm::errs() << "Flowing " << bottleneck << " through path!\n";
  for (auto &E : Path) {
    E->capacity -= bottleneck;
    E->reverse->capacity += bottleneck;
  }
}

std::vector<Edge*> FindPath(uint64_t Node, uint64_t Target,
                           AList_t AdjacencyList, std::set<uint64_t> Visited) {
  Visited.insert(Node);

  for (auto &E : AdjacencyList[Node]) {
    if (Visited.find(E->dst) != Visited.end())
      continue;

    if (E->capacity == 0)
      continue;

    if (E->dst == Target)
      return std::vector<Edge*>{E};

    auto Path = FindPath(E->dst, Target, AdjacencyList, Visited);
    if (Path.size() > 0) {
      Path.insert(Path.begin(), E);
      return Path;
    }
  }

  return std::vector<Edge*>{};
}

std::vector<Edge*> FindPath(AList_t AdjacencyList) {
  return FindPath(0, 1, AdjacencyList, std::set<uint64_t>{});
}

AList_t FlowGraph(AList_t AdjacencyList) {
  std::vector<Edge*> Path = FindPath(AdjacencyList);

  while (Path.size() > 0) {
    for(const auto &E : Path){
      llvm::errs() <<"{src: " << E->src << ", dst: " << E->dst << ", capacity: " << E->capacity << ", residual: "<< E->residual << "}" << "->";
    }
    llvm::errs() << "\n";
    augment(Path);
    Path = FindPath(AdjacencyList);
  }

  return AdjacencyList;
}

void MarkVertices(uint64_t Node, AList_t AdjacencyList,
                  std::set<uint64_t> &Marked) {
  llvm::errs() << "Marking " << Node << "\n";
  Marked.insert(Node);
  for (auto &E : AdjacencyList[Node]) {
    if (Marked.find(E->dst) != Marked.end())
      continue;

    if (E->capacity == 0)
      continue;

    MarkVertices(E->dst, AdjacencyList, Marked);
  }
}

std::set<uint64_t> MarkVertices(AList_t AdjacencyList) {
  std::set<uint64_t> Marked{};

  MarkVertices(0, AdjacencyList, Marked);

  return Marked;
}

std::vector<Edge*> MinCut(AList_t AdjacencyList) {
  auto Marked = MarkVertices(AdjacencyList);

  for(auto i : Marked ) {
    llvm::errs() << i << ", ";
  }
  llvm::errs() << "\n";

  std::vector<Edge*> MinCutEdges{};

  for (const auto &[v, edges] : AdjacencyList) {
    for (const auto &E : edges) {
      if (E->residual)
        continue;

      llvm::errs() << "{src: " << E->src << ", dst: " << E->dst
                   << ", smark: " << (Marked.find(E->src) != Marked.end())
                   << ", dmark: " << (Marked.find(E->dst) == Marked.end())
                   << "}\n";

      if (Marked.find(E->src) != Marked.end() &&
          Marked.find(E->dst) == Marked.end())
        MinCutEdges.push_back(E);
    }
  }

  return MinCutEdges;
}

Node makeGraphUpwards(Instruction *root, Graph &graph) {
  BasicBlock *bb = root->getParent();

  // for inst an instruction before root, going upwards

  bool found_root = false;
  // Reverse iterator to traverse the basic block in reverse order
  for (auto it = bb->rbegin(); it != bb->rend(); ++it) {
    Instruction *inst = &*it;
    if (inst == root) {
      found_root = true;
      continue;
    }
    if (!found_root) {
      continue;
    }

    // If inst is a memory access, then node is getNodeAfter(inst),
    // connectSource(node)
    if (isa<LoadInst>(inst) || isa<StoreInst>(inst)) {
      auto node = Node(inst->getParent(), inst, getOrdering(inst), true);
      graph.addNode(&node);
      graph.addEdge(graph.source, &node);
      return node;
    }
  }

  Node node = getNodeAtBeginning(bb);
  graph.addNode(&node);
  BasicBlock &first_bb_in_func = bb->getParent()->getEntryBlock();
  if (bb == &first_bb_in_func) {
    graph.addEdge(graph.source, &node);
    return node;
  }

  // iterate over all predecessors of bb
  for (auto pred : predecessors(bb)) {
    Node node2 = getNodeAtEnd(pred);
    Instruction *inst2 = getLastInst(pred);

    Node node3 = makeGraphUpwards(inst2, graph);
    if (node3 != NULL) {
      graph.addEdge(&node3, &node2);
      graph.addEdge(&node2, &node);
      return node;
    }
  }
  return NULL; // Do I return NULL here?
  // paper is vague about this
}

Node makeGraphDownwards(Instruction *root, Graph &graph) {
  // Get the basic block of the root instruction.
  BasicBlock *bb = root->getParent();

  // Traverse the instructions in 'bb' starting after 'root'
  bool foundRoot = false;
  for (auto it = bb->begin(); it != bb->end(); ++it) {
    Instruction *inst = &*it;
    if (inst == root) {
      foundRoot = true;
      continue;
    }
    if (!foundRoot)
      continue;

    // Check if the instruction is a memory access or a return.
    if (isa<LoadInst>(inst) || isa<StoreInst>(inst) || isa<ReturnInst>(inst)) {
      // Create a node *before* the instruction.
      // Node *node = new Node(getNodeBefore(inst));
      Node *node = new Node(inst->getParent(), inst, getOrdering(inst), false);
      graph.addNode(node);
      // Connect this node to the sink.
      graph.addEdge(node, graph.sink);
      return *node;
    }
  }

  // No memory access or return instruction was found in the current block.
  // Get the node at the end of the basic block.
  Node *node = new Node(getNodeAtEnd(bb));
  graph.addNode(node);

  // For every successor basic block of the current basic block...
  for (BasicBlock *succ : successors(bb)) {
    // Create a node at the beginning of the successor block.
    Node *node2 = new Node(getNodeAtBeginning(succ));
    graph.addNode(node2);
    // Get the first instruction in the successor basic block.
    Instruction *inst2 = getFirstInst(succ);
    // Recursively build the downward graph starting at inst2.
    Node node3 = makeGraphDownwards(inst2, graph);
    if (node3 != NULL) {
      // Connect the nodes:
      // 1. Connect the current block's end node to the successor's beginning
      // node.
      graph.addEdge(node, node2);
      // 2. Connect the successor beginning node to the node returned by
      // recursion.
      graph.addEdge(node2, &node3);
      return *node;
    }
  }

  // If none of the successor branches return a node, return nullptr.
  return NULL;
}

// #include "llvm/IR/AtomicOrdering.h"

std::string atomicOrderingToString(llvm::AtomicOrdering order) {
  switch (order) {
  case llvm::AtomicOrdering::NotAtomic:
    return "NotAtomic";
  case llvm::AtomicOrdering::Unordered:
    return "Unordered";
  case llvm::AtomicOrdering::Monotonic:
    return "Monotonic";
  case llvm::AtomicOrdering::Acquire:
    return "Acquire";
  case llvm::AtomicOrdering::Release:
    return "Release";
  case llvm::AtomicOrdering::AcquireRelease:
    return "AcquireRelease";
  case llvm::AtomicOrdering::SequentiallyConsistent:
    return "SequentiallyConsistent";
  default:
    return "Unknown";
  }
}

void printGraph(const Graph &graph) {
  llvm::errs() << "Graph Nodes:\n";
  for (int i = 0; i < graph.nodes.size(); ++i) {
    Node *node = graph.nodes[i];
    llvm::errs() << "  Node " << i << ": "
                 << "  Name: " << node->name;
    // << "  Node in BB: " << node->BB->getNumber()
    // if(node -> lastMemOp != nullptr)
    // llvm::errs() << ", Instruction: " << (*node->order);
    // else
    // llvm::errs() << "Instruction: " << "NOP";
    llvm::errs() << ", Ordering: " << atomicOrderingToString(node->order)
                 << ", after: " << (node->after ? "true\n" : "false\n");
  }
  llvm::errs() << "Graph Edges:\n";
  for (auto edge : graph.edges) {
    llvm::errs() << "  Edge from BB: " << edge.first
                 << " to BB: " << edge.second << "\n";
  }
}

// Function TransformFunction(fun)
// Iterates over every fence instruction in the function 'fun', builds the
// upward and downward graphs, and connects them. Finally, it prints the
// resulting graph.
void TransformFunction(Function *fun, Graph &graph) {
  // Iterate over each basic block in the function.
  for (auto &bb : *fun) {
    // Iterate over each instruction in the basic block.
    for (auto &inst : bb) {
      llvm::errs() << inst;
      // Check if the instruction is a fence.
      if (isa<FenceInst>(&inst)) {
        // For the fence instruction, build the upward and downward graphs.
        Node nodeBeforeFence = makeGraphUpwards(&inst, graph);
        Node nodeAfterFence = makeGraphDownwards(&inst, graph);
        // If both nodes exist, connect them.
        if (nodeBeforeFence != nullptr && nodeAfterFence != nullptr) {
          graph.addNode(&nodeBeforeFence);
          graph.addNode(&nodeAfterFence);
          graph.addEdge(&nodeBeforeFence, &nodeAfterFence);
        }
      }
    }
  }

  // Finally, print the graph.
  printGraph(graph);
}

PreservedAnalyses FenceOptimization::run(Module &M, ModuleAnalysisManager &AM) {

  for (Function &F : M.getFunctionList()) {
    errs() << "Function: " << F.getName() << "\n";
    if (F.isDeclaration()) {
      continue;
    }
    Node source = Node(&F.getEntryBlock());
    source.name = 1;
    Node sink = Node(&F.getEntryBlock());
    sink.name = 2;
    Graph graph = Graph();
    graph.source = &source;
    graph.sink = &sink;

    graph.addNode(&source);
    graph.addNode(&sink);

    // Traverse the function to build the graph.
    TransformFunction(&F, graph);

    llvm::errs() << "Building AdjacencyList from Graph!\n";
    AList_t ResidualGraph = graph.buildAdjacencyList(1);
    llvm::errs() << "Flowing Graph!\n";
    FlowGraph(ResidualGraph);
    llvm::errs() << "Extracting Min-Cut from Graph!\n";

    auto MinCutEdges = MinCut(ResidualGraph);

    llvm::errs() << "Number of edges in min-cut: " << MinCutEdges.size();
    for (const auto &E : MinCutEdges) {
      llvm::errs() << "\n  Src: " << E->src << "\n  Dst: " << E->dst
                   << "\n  Capacity: " << E->capacity
                   << "\n  Is Residual?: " << E->residual
                   << "\n  Residual: " << E->reverse->capacity;
    }

    llvm::errs() << "\nInserting fences in new **optimal** positions.\n";

    for (const auto &E : MinCutEdges) {
      if (E->src == 0 || E->dst == 1)
        continue;

      auto src = graph.getNode(E->src);
      auto dst = graph.getNode(E->dst);

      assert(src != nullptr);
      assert(dst != nullptr);

      // TODO: Test
    }
  }

  return PreservedAnalyses::none();
}
