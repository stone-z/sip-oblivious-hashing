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
    void insertHashVariables(int numberOfVariables, llvm::Module& M);
    void dPrint(std::string message);
    void insertSumHashFunction(llvm::Module& M);
    void insertSumOtherHashFunction(llvm::Module& M);    
};

}

