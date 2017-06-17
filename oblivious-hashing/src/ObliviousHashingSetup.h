#pragma once

#include "llvm/Pass.h"

namespace {

class ObliviousHashingSetupPass : public llvm::ModulePass
{
public:
    static char ID;

    ObliviousHashingSetupPass()
        : llvm::ModulePass(ID)
    {
    }

    bool runOnModule(llvm::Module& M) override;
    virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

}

