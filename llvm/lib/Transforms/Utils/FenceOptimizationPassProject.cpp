#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/FenceOptimizationPassProject.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/AtomicOrdering.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// void TraverseBBGraphFenceOptPass(BasicBlock &BB, AtomicOrdering order, Instruction *lastMemOp) {
//   if (BB.empty()) {
//     llvm::errs() << "Basic Block is empty.\n";
//     return;
//   }

//   // Print which basic block is being traversed
//   llvm::errs() << "Traversing Basic Block: " << BB.getName() << "\n";

//   for (Instruction &I : BB) {
//     llvm::errs() << "Processing instruction: " << I << "\n";

//     // Handle Load instructions
//     if (auto *Load = dyn_cast<LoadInst>(&I)) {
//       if(lastMemOp == nullptr) {
//         lastMemOp = &I;
//         continue;
//       }

//       AtomicOrdering loadOrder = Load->getOrdering();
//       llvm::errs() << "  Found Load instruction with ordering: " << (unsigned)loadOrder << "\n";

//       // Skip if load has ordering other than Unordered, Monotonic, or Acquire
//       if (loadOrder != AtomicOrdering::Unordered &&
//           loadOrder != AtomicOrdering::Monotonic &&
//           loadOrder != AtomicOrdering::Acquire) {
//         llvm::errs() << "    Skipping load: unsupported ordering. "
//                         "Setting order to Unordered and updating lastMemOp.\n";
//         order = AtomicOrdering::Unordered;
//         lastMemOp = &I;
//         continue;
//       }

//       // Insert fence before this load if previous memory op was a Load and ordering is too weak
//       if (isa<StoreInst>(lastMemOp)) {
//         llvm::errs() << "    Previous memory op is a Store. No fence inserted.\n";
//       } else if (isa<LoadInst>(lastMemOp) &&
//                  order != AtomicOrdering::Acquire &&
//                  order != AtomicOrdering::AcquireRelease &&
//                  order != AtomicOrdering::SequentiallyConsistent) {
//         llvm::errs() << "    Inserting fence before Load due to ordering constraints.\n";
//         IRBuilder<> Builder(&I);
//         Builder.CreateFence(AtomicOrdering::Acquire);
//       }

//       order = loadOrder;
//       lastMemOp = &I;
//     }

//     // Handle Store instructions
//     else if (auto *Store = dyn_cast<StoreInst>(&I)) {
//       if(lastMemOp == nullptr) {
//         lastMemOp = &I;
//         continue;
//       }

//       AtomicOrdering storeOrder = Store->getOrdering();
//       llvm::errs() << "  Found Store instruction with ordering: " << (unsigned)storeOrder << "\n";

//       // Insert fence if needed based on the previous memory op
//       if (isa<StoreInst>(lastMemOp) &&
//           order != AtomicOrdering::Release &&
//           order != AtomicOrdering::AcquireRelease &&
//           order != AtomicOrdering::SequentiallyConsistent) {
//         llvm::errs() << "    Inserting fence before Store (following a Store) due to ordering constraints.\n";
//         IRBuilder<> Builder(&I);
//         Builder.CreateFence(AtomicOrdering::Release);
//       } else if (isa<LoadInst>(lastMemOp) &&
//                  order != AtomicOrdering::Acquire &&
//                  order != AtomicOrdering::AcquireRelease &&
//                  order != AtomicOrdering::SequentiallyConsistent) {
//         llvm::errs() << "    Inserting fence before Store (following a Load) due to ordering constraints.\n";
//         IRBuilder<> Builder(&I);
//         Builder.CreateFence(AtomicOrdering::Acquire);
//       }

//       order = storeOrder;
//       lastMemOp = &I;
//     }

//     // Handle Fence instructions
//     else if (auto *Fence = dyn_cast<FenceInst>(&I)) {
//       llvm::errs() << "  Encountered Fence instruction with ordering: " << (unsigned)Fence->getOrdering() << "\n";
//       order = Fence->getOrdering();
//     }

//     // Handle terminator instructions and recursively traverse successors
//     else if (I.isTerminator()) {
//       llvm::errs() << "  Encountered Terminator instruction.\n";
//       if (auto *BI = dyn_cast<BranchInst>(&I)) {
//         llvm::errs() << "    Branch instruction with " << BI->getNumSuccessors() << " successors.\n";
//         for (unsigned i = 0; i < BI->getNumSuccessors(); ++i) {
//           BasicBlock *Successor = BI->getSuccessor(i);
//           llvm::errs() << "      Traversing successor Basic Block: " << Successor->getName() << "\n";
//           TraverseBBGraph(*Successor, order, lastMemOp);
//         }
//       } else if (auto *SI = dyn_cast<SwitchInst>(&I)) {
//         llvm::errs() << "    Switch instruction with " << SI->getNumSuccessors() << " successors.\n";
//         for (unsigned i = 0; i < SI->getNumSuccessors(); ++i) {
//           BasicBlock *Successor = SI->getSuccessor(i);
//           llvm::errs() << "      Traversing successor Basic Block: " << Successor->getName() << "\n";
//           TraverseBBGraph(*Successor, order, lastMemOp);
//         }
//       } // TODO Might need to handle calls here as well
//     }
//   }
// }

AtomicOrdering getOrdering(Instruction *inst) {

  if (isa<LoadInst>(inst)) {
    return cast<LoadInst>(&inst)->getOrdering();
  } else if (isa<StoreInst>(inst)) {
    return cast<StoreInst>(&inst)->getOrdering();
  } else if (isa<FenceInst>(inst)) {
    return cast<FenceInst>(&inst)->getOrdering();
  }
  return AtomicOrdering::NotAtomic;
}

struct Node {
  BasicBlock *BB;
  Instruction *lastMemOp = nullptr;
  AtomicOrdering order = AtomicOrdering::NotAtomic;
  // Tells you if the node is after the last memory operation, place a fence accordingly
  bool after = false;
  std::vector<Node *> successors;
  std::vector<Node *> predecessors;

  Node(BasicBlock *bb) : BB(bb) {} // Can I have default constructor?
  Node(BasicBlock *bb, Instruction *lastMemOp, AtomicOrdering order)
      : BB(bb), lastMemOp(lastMemOp), order(order) {}
  Node(BasicBlock *bb, Instruction *lastMemOp, AtomicOrdering order, bool after)
      : BB(bb), lastMemOp(lastMemOp), order(order), after(after) {}

  Node getNodeAfter(Instruction *inst) {
    Node node = Node(inst->getParent(), inst, getOrdering(inst), true);
    return node;
  }

  Node getNodeBefore(Instruction *inst) {
    Node node = Node(inst->getParent(), inst, getOrdering(inst), false);
    return node;
  }


  bool operator==(const Node &other) const {
    return BB == other.BB && lastMemOp == other.lastMemOp && order == other.order && after == other.after;
  }

  bool operator!=(const Node &other) const {
    return !(*this == other);
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

Instruction* getLastInst(BasicBlock *bb) {
  Instruction *lastInst = &*bb->rbegin();
  return lastInst;
}

Instruction* getFirstInst(BasicBlock *bb) {
  Instruction *firstInst = &*bb->begin();
  return firstInst;
}


struct Graph {
  Node *source;
  Node *sink;
  std::vector<Node *> nodes;
  std::vector<std::pair<Node *, Node *>> edges;

  void addNode(Node *node) {
    for (auto existingNode : nodes) {
      if (*existingNode == *node) {
        return;
      }
    }
    nodes.push_back(node);
  }

  void addEdge(Node *from, Node *to) {
    edges.emplace_back(from, to);
    from->successors.push_back(to);
    to->predecessors.push_back(from);
  }
};


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

    // If inst is a memory access, then node is getNodeAfter(inst), connectSource(node)
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
    Node* node2 = new Node(getNodeAtBeginning(succ));
    graph.addNode(node2);
    // Get the first instruction in the successor basic block.
    Instruction *inst2 = getFirstInst(succ);
    // Recursively build the downward graph starting at inst2.
    Node node3 = makeGraphDownwards(inst2, graph);
    if (node3 != NULL) {
      // Connect the nodes:
      // 1. Connect the current block's end node to the successor's beginning node.
      graph.addEdge(node, node2);
      // 2. Connect the successor beginning node to the node returned by recursion.
      graph.addEdge(node2, &node3);
      return *node;
    }
  }

  // If none of the successor branches return a node, return nullptr.
  return NULL;
}

// #include "llvm/IR/AtomicOrdering.h"
#include <string>

std::string atomicOrderingToString(llvm::AtomicOrdering order) {
  switch (order) {
    case llvm::AtomicOrdering::NotAtomic:            return "NotAtomic";
    case llvm::AtomicOrdering::Unordered:              return "Unordered";
    case llvm::AtomicOrdering::Monotonic:              return "Monotonic";
    case llvm::AtomicOrdering::Acquire:                return "Acquire";
    case llvm::AtomicOrdering::Release:                return "Release";
    case llvm::AtomicOrdering::AcquireRelease:         return "AcquireRelease";
    case llvm::AtomicOrdering::SequentiallyConsistent: return "SequentiallyConsistent";
    default:                                           return "Unknown";
  }
}

void printGraph(const Graph &graph) {
  llvm::errs() << "Graph Nodes:\n";
  for (auto node : graph.nodes) {
    llvm::errs() << "  Node in BB: " << node->BB->getNumber()
                 << ", LastMemOp: "
                 << (node->lastMemOp ? node->lastMemOp->getOpcodeName() : "None");
    if(node -> lastMemOp != nullptr)
       llvm::errs() << ", Instruction: " << (*node->lastMemOp);
    else
      llvm::errs() << "Instruction: " << "NOP";
    llvm::errs() << ", Ordering: " << atomicOrderingToString(node->order)
                 << ", after: " << (node->after ? "true\n" : "false\n");
  }
  llvm::errs() << "Graph Edges:\n";
  for (auto edge : graph.edges) {
    llvm::errs() << "  Edge from BB: " << edge.first->BB->getNumber()
                 << " to BB: " << edge.second->BB->getNumber() << "\n";
  }
}

// Function TransformFunction(fun)
// Iterates over every fence instruction in the function 'fun', builds the upward and downward graphs,
// and connects them. Finally, it prints the resulting graph.
void TransformFunction(Function *fun, Graph &graph) {
  // Iterate over each basic block in the function.
  for (auto &bb : *fun) {
    // Iterate over each instruction in the basic block.
    for (auto &inst : bb) {
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


PreservedAnalyses FenceOptimizationPassProject::run(Module &M,
                                      ModuleAnalysisManager &AM) {


  for( Function &F : M.getFunctionList() ) {
    errs() << "Function: " << F.getName() << "\n";
    if (F.isDeclaration()) {
      continue;
    }
    Node source = Node(&F.getEntryBlock());
    Node sink = Node(&F.getEntryBlock());
    Graph graph = Graph();
    graph.source = &source;
    graph.sink = &sink;

    graph.addNode(&source);
    graph.addNode(&sink);

    // Traverse the function to build the graph.
    TransformFunction(&F, graph);
  }

  return PreservedAnalyses::none();
}

/*
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "MyModulePass", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "fence-tso") {
                        MPM.addPass(FenceTSOPass);
                        return true;
                    }
                    return false;
                });
        }
    };
}
*/
