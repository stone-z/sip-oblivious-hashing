#include "InputDependencySkeleton.h"

#include "input-dependency/InputDependencyAnalysis.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

namespace skeleton {

char InputDependencySkeletonPass::ID = 0;

void InputDependencySkeletonPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const
{
    AU.setPreservesAll();
    AU.addRequired<input_dependency::InputDependencyAnalysis>();
}

bool InputDependencySkeletonPass::runOnModule(llvm::Module& M)
{
    const auto& input_dependency_info = getAnalysis<input_dependency::InputDependencyAnalysis>();
    for (auto& F : M) {
        // No input dependency info for declarations.
        if (F.isDeclaration()) {
            continue;
        }
        for (auto& B : F) {
            for (auto& I : B) {
                if (input_dependency_info.isInputDependent(&I)) {
                    llvm::dbgs() << "Instruction: " << I << " is input dependent\n";
                }
            }
        }
    }
    return false;
}

static llvm::RegisterPass<InputDependencySkeletonPass> X("input-dep-skeleton","Reports input dependent instructions in bitcode");

static void registerPathsAnalysisPass(const llvm::PassManagerBuilder &,
                         	      llvm::legacy::PassManagerBase &PM) {
  PM.add(new InputDependencySkeletonPass());
}

static llvm::RegisterStandardPasses RegisterMyPass(llvm::PassManagerBuilder::EP_EarlyAsPossible, registerPathsAnalysisPass);


}

