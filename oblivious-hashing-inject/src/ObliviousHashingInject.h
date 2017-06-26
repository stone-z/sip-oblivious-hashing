#pragma once

#include "llvm/Pass.h"
#include <unordered_set>
#include <iostream>

using namespace std;

namespace {

class ObliviousHashingInjectPass : public llvm::ModulePass
{
public:
    static char ID;

    ObliviousHashingInjectPass()
        : llvm::ModulePass(ID)
    {
    }

    bool runOnModule(llvm::Module& M) override;
    //virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
    std::unordered_set<std::string> insertHashVariables(int numberOfVariables, llvm::Module& M);
    // void insertHashVariables(int numberOfVariables, llvm::Module& M);
    void dPrint(std::string message);
    void insertSumHashFunction(llvm::Module& M);
    void insertSumOtherHashFunction(llvm::Module& M);
    // void insertRandomly(llvm::Module& M, int numberOfVariables, int numberOfChecks);    
    void insertRandomly(llvm::Module& M/*, std::unordered_set<std::string> listOfVariables*/, int numberOfChecks);    
    void insertAssertion(llvm::Module& M);
};

}

