#include "ObliviousHashingSetup.h"

// #include "input-dependency/InputDependencyAnalysis.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"

using namespace llvm;
using namespace std;

namespace {

char ObliviousHashingSetupPass::ID = 0;

void ObliviousHashingSetupPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const
{
    AU.setPreservesAll();
    // AU.addRequired<input_dependency::InputDependencyAnalysis>();
}

bool ObliviousHashingSetupPass::runOnModule(llvm::Module& M)
{
    errs() << "Pass is running" << '\n';
    
    // Insert at least 2 different hash functions

    // Insert assertions at random points in the program
    return false;
}

void insertHashFunction(){

}





static llvm::RegisterPass<ObliviousHashingSetupPass> X("oh-setup-pass","Prepares a program for oblivious hashing instrumentation.");

static void registerPathsAnalysisPass(const llvm::PassManagerBuilder &,
                         	      llvm::legacy::PassManagerBase &PM) {
  PM.add(new ObliviousHashingSetupPass());
}

static llvm::RegisterStandardPasses RegisterMyPass(llvm::PassManagerBuilder::EP_EarlyAsPossible, registerPathsAnalysisPass);


}

