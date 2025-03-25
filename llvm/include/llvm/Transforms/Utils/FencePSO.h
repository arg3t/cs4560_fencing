#ifndef LLVM_TRANSFORMS_FENCENEW_FENCEPSO_H
#define LLVM_TRANSFORMS_FENCENEW_FENCEPSO_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class FencePSOPass : public PassInfoMixin<FencePSOPass> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_FENCENEW_FENCEPSO_H
