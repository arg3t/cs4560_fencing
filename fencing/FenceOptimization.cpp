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

class Node {
public:
  BasicBlock *BB;
  WeakTrackingVH lastMemOp = NULL;
  AtomicOrdering order = AtomicOrdering::NotAtomic;
  int name = 0;
  // Tells you if the node is after the last memory operation, place a fence
  // accordingly
  bool after = false;
  std::vector<Node *> successors;
  std::vector<Node *> predecessors;
  Node(BasicBlock *bb) : BB(bb), name(0) {} // Can I have default constructor?
  Node(BasicBlock *bb, WeakTrackingVH lastMemOp, AtomicOrdering order)
      : BB(bb), lastMemOp(lastMemOp), order(order), name(0) {}
  Node(BasicBlock *bb, WeakTrackingVH lastMemOp, AtomicOrdering order,
       bool after)
      : BB(bb), lastMemOp(lastMemOp), order(order), after(after), name(0) {}

  Node(BasicBlock *bb, Instruction *lastMemOp, AtomicOrdering order,
       bool after) {
    this->BB = bb;
    this->lastMemOp = WeakTrackingVH(lastMemOp);
    this->order = order;
    this->after = after;
    this->name = 0;
  }

  Node getNodeAfter(Instruction *inst) {
    auto track = WeakTrackingVH(inst);
    Node node = Node(inst->getParent(), track, getOrdering(inst), true);
    return node;
  }

  Node getNodeBefore(Instruction *inst) {
    auto track = WeakTrackingVH(inst);
    Node node = Node(inst->getParent(), track, getOrdering(inst), false);
    return node;
  }

  bool operator==(const Node &other) const {
    // return BB == other.BB && lastMemOp == other.lastMemOp &&
    //        order == other.order && after == other.after && name ==
    //        other.name;

    std::string thisStr, otherStr;
    // Use llvm's raw_string_ostream to capture the output in strings
    llvm::raw_string_ostream thisStream(thisStr);
    llvm::raw_string_ostream otherStream(otherStr);
    thisStream << *this;
    otherStream << other;
    // Compare the resulting string representations
    return thisStream.str() == otherStream.str();
  }

  bool operator!=(const Node &other) const { return !(*this == other); }

  friend raw_ostream &operator<<(raw_ostream &os, const Node &node) {

    // llvm::errs() << "  Node " << i << ": "
    // << "  Name: " << node->name;
    // // << "  Node in BB: " << node->BB->getNumber()
    // if(node -> lastMemOp != NULL)
    // llvm::errs() << ", Instruction: " << *(node->lastMemOp);
    // else
    // llvm::errs() << "Instruction: " << "NOP";
    // llvm::errs() << ", Ordering: " << atomicOrderingToString(node->order)
    //              << ", after: " << (node->after ? "true\n" : "false\n");

    // llvm::errs() << "Node " << i << " : " << *node << "\n";

    auto op_string = "none";

    if (node.lastMemOp != NULL) {
      os << "{BB: " << node.BB << ", Instruction: " << *(node.lastMemOp)
         << ", order: " << atomicOrderingToString(node.order)
         << ", after: " << (node.after ? "true" : "false")
         << ", name: " << node.name << "}";
      return os;
    } else {
      os << "{BB: " << node.BB << ", Instruction: " << "None"
         << ", order: " << atomicOrderingToString(node.order)
         << ", after: " << (node.after ? "true" : "false")
         << ", name: " << node.name << "}";
      return os;
    }
  }
};

struct Edge {
  uint64_t src;
  uint64_t dst;

  uint32_t capacity;
  bool residual;

  Edge *reverse;

  Edge(uint64_t src, uint64_t dst, uint32_t capacity, bool residual,
       Edge *reverse)
      : src(src), dst(dst), capacity(capacity), residual(residual),
        reverse(reverse) {};

  Edge(uint64_t src, uint64_t dst, uint32_t capacity, bool residual)
      : src(src), dst(dst), capacity(capacity), residual(residual),
        reverse(nullptr) {};

  std::ostream &operator<<(std::ostream &os) {
    os << "{src: " << src << ", dst: " << dst << ", capacity: " << capacity
       << ", residual: " << residual << "}";
    return os;
  }
};

using AList_t = std::map<uint64_t, std::vector<Edge *>>;

struct Graph {
  Node *source;
  Node *sink;

  std::vector<Node *> nodes;
  std::vector<std::pair<uint64_t, uint64_t>> edges;

  Node *addNode(Node *node) {
    for (auto existingNode : nodes) {

      // llvm::errs()<< "Comparing nodes:\n";
      // llvm::errs() << "Node: " << *node << "\n";
      // llvm::errs() << "Existing Node: " << *existingNode << "\n";

      if (*existingNode == *node) {
        // llvm::errs() << "DEBUG: Node already exists in graph.\n";
        // llvm::errs() << "Node: " << *node << "\n";
        // llvm::errs() << "Existing Node: " << *existingNode << "\n";
        return existingNode;
      }
    }
    nodes.push_back(node);
    return node;
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
      llvm::errs() << "From instruction: " << *from
                   << " With index: " << fromIndex << "\n";
      llvm::errs() << "To instruction: " << *to << " With index: " << fromIndex
                   << "\n";
      return;
    }

    if (fromIndex == toIndex) {
      llvm::errs() << "Error: Cannot add edge from node to itself.\n";
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

  int findNode(Node *node) {
    for (size_t i = 0; i < nodes.size(); ++i) {
      if (*nodes[i] == *node) {
        return i;
      }
    }
    return -1;
  }
};

Node *getNodeAtBeginning(BasicBlock *bb) {
  Instruction *firstInst = &*bb->begin();

  // if firstint is a fence instruction, then use the next instruction
  if (isa<FenceInst>(firstInst)) {
    firstInst = &*std::next(bb->begin());
  }

  Node *node = new Node(bb, firstInst, getOrdering(firstInst), false);
  node->after = false;
  return node;
}

Node *getNodeAtEnd(BasicBlock *bb) {
  Instruction *lastInst = &*bb->rbegin();

  // if lastInst is a fence instruction, then use the previous instruction
  if (isa<FenceInst>(lastInst)) {
    lastInst = &*std::prev(bb->rend());
  }

  Node *node = new Node(bb, lastInst, getOrdering(lastInst), true);
  node->after = true;
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

void augment(std::vector<Edge *> Path) {
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

std::vector<Edge *> FindPath(uint64_t Node, uint64_t Target,
                             AList_t AdjacencyList,
                             std::set<uint64_t> Visited) {
  Visited.insert(Node);

  for (auto &E : AdjacencyList[Node]) {
    if (Visited.find(E->dst) != Visited.end())
      continue;

    if (E->capacity == 0)
      continue;

    if (E->dst == Target)
      return std::vector<Edge *>{E};

    auto Path = FindPath(E->dst, Target, AdjacencyList, Visited);
    if (Path.size() > 0) {
      Path.insert(Path.begin(), E);
      return Path;
    }
  }

  return std::vector<Edge *>{};
}

std::vector<Edge *> FindPath(AList_t AdjacencyList) {
  return FindPath(0, 1, AdjacencyList, std::set<uint64_t>{});
}

AList_t FlowGraph(AList_t AdjacencyList) {
  std::vector<Edge *> Path = FindPath(AdjacencyList);

  while (Path.size() > 0) {
    for (const auto &E : Path) {
      llvm::errs() << "{src: " << E->src << ", dst: " << E->dst
                   << ", capacity: " << E->capacity
                   << ", residual: " << E->residual << "}" << "->";
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

std::vector<Edge *> MinCut(AList_t AdjacencyList) {
  auto Marked = MarkVertices(AdjacencyList);

  for (auto i : Marked) {
    llvm::errs() << i << ", ";
  }
  llvm::errs() << "\n";

  std::vector<Edge *> MinCutEdges{};

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

Node *makeGraphUpwards(Instruction *root, Graph &graph) {
  // basicBlock ←GetBasicBlock(root)
  llvm::errs() << "\n";
  llvm::errs() << "Root instruction: " << *root << "\n";
  auto *bb = root->getParent();
  // for inst an instruction before root in basicBlock, going upwards do
  bool foundRoot = false;
  for (auto it = bb->rbegin(); it != bb->rend(); it++) {
    Instruction *inst = &*it;
    if (inst == root) {
      foundRoot = true;
      continue;
    }
    if (!foundRoot)
      continue;

    llvm::errs() << "Found instruction: " << *inst << "\n";
    // if inst a memory access then
    if (isa<LoadInst>(inst) || isa<StoreInst>(inst)) {
      // node ←GetNodeAfter(inst)


      if(isa<LoadInst>(inst)) {
        LoadInst *loadInst = cast<LoadInst>(inst);
        AtomicOrdering order = loadInst->getOrdering();
        if (order == AtomicOrdering::NotAtomic) {
          continue;
        }
      } else if(isa<StoreInst>(inst)) {
        StoreInst *storeInst = cast<StoreInst>(inst);
        AtomicOrdering order = storeInst->getOrdering();
        if (order == AtomicOrdering::NotAtomic) {
          continue;
        }
      }

      llvm::errs() << "\n  Found memory access: " << *inst << "\n";
      Node *node = new Node(inst->getParent(), inst, getOrdering(inst), true);

      graph.addNode(node);
      // connect node to source
      graph.addEdge(graph.source, node);
      return node;
    }
  }

  Node *node = getNodeAtBeginning(bb);
  node = graph.addNode(node);

  // if (!added) {
  //   llvm::errs() << "Error: Node already exists in graph.\n";
  //   llvm::errs() << "Node: " << node << "\n";
  //   return node;
  // }

  // if basic block is first in function
  if (pred_empty(bb)) { // Check if the basic block has no predecessors
    llvm::errs() << "First basic block in function.\n";
    llvm::errs() << "Node: " << node << "\n";
    graph.addEdge(graph.source, node);
    return node;
  }

  // else connect node to the previous basic block
  for (BasicBlock *pred : predecessors(bb)) {
    Node *node2 = getNodeAtEnd(pred);
    graph.addNode(node2);
    auto *lastInst = getLastInst(pred);

    // Recursively build the upward graph starting at lastInst.
    Node *node3 = makeGraphUpwards(lastInst, graph);

    if (node3 != nullptr) {
      // Connect the nodes:
      // 1. Connect the current block's beginning node to the predecessor's end
      // node.
      graph.addEdge(node2, node);
      // 2. Connect the predecessor end node to the node returned by recursion.
      graph.addNode(node3);
      graph.addEdge(node3, node2);
    }
  }
  return node;
  llvm::errs() << "\n\n\nNo memory access found in the upward graph.\n\n\n";
  return nullptr;
}

Node *makeGraphDownwards(Instruction *root, Graph &graph) {
  // Get the basic block of the root instruction.
  BasicBlock *bb = root->getParent();

  // Iterate over the instructions in the basic block, going downwards.
  bool foundRoot = false;
  for (auto it = bb->begin(); it != bb->end(); it++) {
    Instruction *inst = &*it;
    if (inst == root) {
      foundRoot = true;
      continue;
    }
    if (!foundRoot)
      continue;

    // If inst is a memory access, create a node.
    if (isa<LoadInst>(inst) || isa<StoreInst>(inst) || isa<ReturnInst>(inst)) {

      if(isa<LoadInst>(inst)) {
        LoadInst *loadInst = cast<LoadInst>(inst);
        AtomicOrdering order = loadInst->getOrdering();
        if (order == AtomicOrdering::NotAtomic) {
          continue;
        }
      } else if(isa<StoreInst>(inst)) {
        StoreInst *storeInst = cast<StoreInst>(inst);
        AtomicOrdering order = storeInst->getOrdering();
        if (order == AtomicOrdering::NotAtomic) {
          continue;
        }
      }

      llvm::errs() << "Found memory access or return: " << *inst << "\n";
      Node *node = new Node(inst->getParent(), inst, getOrdering(inst), false);
      graph.addNode(node);
      // Connect the node to the sink.
      graph.addEdge(node, graph.sink);
      return node;
    }
  }

  Node *node = getNodeAtEnd(bb);
  node = graph.addNode(node);

  // if (!added) {
  //   llvm::errs() << "Error: Node already exists in graph.\n";
  //   llvm::errs() << "Node: " << node << "\n";
  //   return node;
  // }

  // If the basic block is the last in the function, connect it to the sink.
  if (succ_empty(bb)) { // Check if the basic block has no successors
    llvm::errs() << "Last basic block in function.\n";
    graph.addEdge(node, graph.sink);
    return node;
  }

  for (BasicBlock *succ : successors(bb)) {
    Node *node2 = getNodeAtBeginning(succ);
    graph.addNode(node2);
    auto *firstInst = getFirstInst(succ);
    llvm::errs() << "First instruction in successor: " << *firstInst << "\n";
    // Recursively build the downward graph starting at firstInst.
    Node *node3 = makeGraphDownwards(firstInst, graph);

    if (node3 != nullptr) {
      // Connect the nodes:
      // 1. Connect the current block's end node to the successor's beginning
      // node.
      graph.addEdge(node, node2);
      // 2. Connect the successor beginning node to the node returned by
      // recursion.
      graph.addNode(node3);
      graph.addEdge(node2, node3);
    }
  }

  return node;
  // If none of the successor branches return a node, return nullptr.
  return nullptr;
}

// #include "llvm/IR/AtomicOrdering.h"

void printGraph(Graph &graph) {
  llvm::errs() << "Graph Nodes:\n";
  for (int i = 0; i < graph.nodes.size(); ++i) {
    Node *node = graph.nodes[i];
    llvm::errs() << "Node " << i << " : " << *node << "\n";
  }
  llvm::errs() << "Graph Edges:\n";
  for (auto edge : graph.edges) {
    llvm::errs() << "  Edge from Node: " << edge.first
                 << " to Node: " << edge.second << "\n";
  }
}

// Function TransformFunction(fun)
// Iterates over every fence instruction in the function 'fun', builds the
// upward and downward graphs, and connects them. Finally, it prints the
// resulting graph.
void TransformFunction(Function *fun, Graph &graph) {

  llvm::errs() << "\n\n\n------------------\nTransforming function: "
               << fun->getName() << "\n";

  // Iterate over each basic block in the function.
  for (auto &bb : *fun) {
    // llvm::errs() << "Basic Block: " << bb.getNumber() << "\n";
    // Iterate over each instruction in the basic block.
    for (auto &inst : bb) {
      llvm::errs() << " :::::::::Inst: " << inst << "\n";
      // Check if the instruction is a fence.
      if (isa<FenceInst>(&inst)) {
        // For the fence instruction, build the upward and downward graphs.
        Node *nodeBeforeFence = makeGraphUpwards(&inst, graph);

        llvm::errs() << "Made graph upwards.\n";
        printGraph(graph);

        Node *nodeAfterFence = makeGraphDownwards(&inst, graph);

        llvm::errs() << "Made graph downwards.\n";
        printGraph(graph);

        // If both nodes exist, connect them.
        if (nodeBeforeFence != nullptr && nodeAfterFence != nullptr) {
          graph.addNode(nodeBeforeFence);
          graph.addNode(nodeAfterFence);
          graph.addEdge(nodeBeforeFence, nodeAfterFence);
        }
      }
    }
  }

  // Finally, print the graph.
  printGraph(graph);

  for (auto bbIter = fun->begin(), bbEnd = fun->end(); bbIter != bbEnd;
       ++bbIter) {
    BasicBlock &bb = *bbIter;
    for (auto instIter = bb.begin(), instEnd = bb.end(); instIter != instEnd;) {
      Instruction &inst = *instIter;
      // Advance the iterator before removal if needed.
      ++instIter;

      if (isa<FenceInst>(&inst)) {
        llvm::errs() << "\nRemoved fence instruction: " << inst << "\n";
        inst.eraseFromParent();
        llvm::errs() << "After removing fence instruction.\n";
      }
    }
  }

  // Print the graph after removing the fence instruction.
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
      // if (E->src == 0 || E->dst == 1)
      //   continue;

      if(E->src == 0){
        auto dst = graph.getNode(E->dst);

        assert(dst != nullptr);

        LLVMContext &context = dst->BB->getContext();
        FenceInst *newFence = new FenceInst(
            dst->BB->getContext(), AtomicOrdering::SequentiallyConsistent);
        Instruction *lastInst = dyn_cast<Instruction>(dst->lastMemOp);
        llvm::errs() << "Inserting fence before instruction: " << *lastInst
                     << "\n";
        newFence->insertBefore(lastInst);
        continue;
      }

      auto src = graph.getNode(E->src);
      auto dst = graph.getNode(E->dst);

      assert(src != nullptr);
      assert(dst != nullptr);

      LLVMContext &context = src->BB->getContext();
      FenceInst *newFence = new FenceInst(
          src->BB->getContext(), AtomicOrdering::SequentiallyConsistent);

      Instruction *lastInst = dyn_cast<Instruction>(src->lastMemOp);
      llvm::errs() << "Inserting fence after instruction: " << *lastInst
                   << "\n";
      newFence->insertAfter(lastInst);
    }
  }

  return PreservedAnalyses::none();
}
