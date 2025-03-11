#include "llvm/Transforms/Utils/FenceTSO.h"
#include "llvm/IR/Function.h"

using namespace llvm;

PreservedAnalyses FenceTSOPass::run(Function &F,
                                      FunctionAnalysisManager &AM) {
  errs() << F.getName() << "\n";
  return PreservedAnalyses::all();
}
