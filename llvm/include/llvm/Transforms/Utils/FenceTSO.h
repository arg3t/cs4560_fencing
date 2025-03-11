#ifndef LLVM_TRANSFORMS_FENCENEW_FENCETSO_H
#define LLVM_TRANSFORMS_FENCENEW_FENCETSO_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class FenceTSOPass : public PassInfoMixin<FenceTSOPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_FENCENEW_FENCETSO_H
