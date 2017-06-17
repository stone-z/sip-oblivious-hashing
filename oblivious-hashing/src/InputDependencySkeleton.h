#pragma once

#include "llvm/Pass.h"

namespace skeleton {

class InputDependencySkeletonPass : public llvm::ModulePass
{
public:
    static char ID;

    InputDependencySkeletonPass()
        : llvm::ModulePass(ID)
    {
    }

    bool runOnModule(llvm::Module& M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

}

