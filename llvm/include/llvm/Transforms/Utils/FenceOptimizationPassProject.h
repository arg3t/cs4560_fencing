#ifndef LLVM_TRANSFORMS_FENCENEW_FENCEOPTIMIZATIONPASSPROJECT_H
#define LLVM_TRANSFORMS_FENCENEW_FENCEOPTIMIZATIONPASSPROJECT_H

#include "llvm/IR/PassManager.h"

namespace llvm {

class FenceOptimizationPassProject : public PassInfoMixin<FenceOptimizationPassProject> {
public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_FENCENEW_FENCEOPTIMIZATIONPASSPROJECT_H
