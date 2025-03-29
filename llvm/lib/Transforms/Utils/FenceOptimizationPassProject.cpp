#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
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

// Enum of node types, source, sink and fence
enum NodeType {
  Source,
  Sink,
  Fence
};


// Fence Node structure stores parent BasicBlock, reference to fence, fence ordering (release/acquire/seq_cst)
// Sink Node stores nothing
// Source Node stores nothing 

struct Node {
  NodeType type;
  BasicBlock *parentBB;
  Instruction *fenceInst;
  AtomicOrdering fenceOrder;

  Node(NodeType t, BasicBlock *bb, Instruction *fence, AtomicOrdering order)
      : type(t), parentBB(bb), fenceInst(fence), fenceOrder(order) {}
};

struct Edge {
  Node *source;
  Node *sink;
  int weight;
  Edge(Node *s, Node *d, int w) : source(s), sink(d), weight(w) {}

};
// Graph structure to hold nodes and edges

class Graph {
public:
  Node *sourceNode;
  Node *sinkNode;
  std::vector<Node *> nodes;
  std::vector<Edge *> edges;

  Graph(Node *source, Node *sink) : sourceNode(source), sinkNode(sink) {
    nodes.push_back(source);
    nodes.push_back(sink);
  }
  ~Graph() {
    for (auto edge : edges) {
      delete edge;
    }
    for (auto node : nodes) {
      delete node;
    }
  }

  void addNode(Node *node) {
    nodes.push_back(node);
  }

  void addEdge(Node *source, Node *sink, int weight) {
    edges.push_back(new Edge(source, sink, weight));
  }

  void printGraph() {
    for (auto node : nodes) {
      llvm::errs() << "Node Type: " << node->type << "\n";
      llvm::errs() << "Parent BB: " << node->parentBB->getName() << "\n";
      if (node->fenceInst) {
        llvm::errs() << "Fence Inst: " << *node->fenceInst << "\n";
        llvm::errs() << "Fence Order: " << (unsigned)node->fenceOrder << "\n";
      }
    }
  }
};



// For each function, traverse BB. 
// Create a graph, add nodes and edges for all fence instructions.

Node *sourceNode = new Node(Source, nullptr, nullptr, AtomicOrdering::NotAtomic);
Node *sinkNode = new Node(Sink, nullptr, nullptr, AtomicOrdering::NotAtomic);

Graph *graph = nullptr;

void traverseFunction(Function &F) {
  Graph *graph = new Graph(nullptr, nullptr);
  Node *sourceNode = new Node(Source, nullptr, nullptr, AtomicOrdering::NotAtomic);
  Node *sinkNode = new Node(Sink, nullptr, nullptr, AtomicOrdering::NotAtomic);
  graph->addNode(sourceNode);
  graph->addNode(sinkNode);

  for (BasicBlock &BB : F) {
    llvm::errs() << "Processing Basic Block: " << BB.getName() << "\n";
    for (Instruction &I : BB) {
      if (auto *Fence = dyn_cast<FenceInst>(&I)) {
        llvm::errs() << "Found Fence instruction: " << *Fence << "\n";
        // Node *fenceNode = new Node(NodeType::Fence, &BB, Fence, Fence->getOrdering());
        // graph->addNode(fenceNode);
        // graph->addEdge(sourceNode, fenceNode, 1); // Example weight
      }
    }
  }

  // graph->printGraph();
  // delete graph;
}



PreservedAnalyses FenceOptimizationPassProject::run(Module &M,
                                      ModuleAnalysisManager &AM) {


  for( Function &F : M.getFunctionList() ) {
    errs() << "Function: " << F.getName() << "\n";
    if (F.isDeclaration()) {
      continue;
    }
    graph = new Graph(sourceNode, sinkNode);
    // Traverse the function and create a graph
    traverseFunction(F);
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
