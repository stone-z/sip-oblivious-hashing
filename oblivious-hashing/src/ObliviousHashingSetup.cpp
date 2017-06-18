#include "ObliviousHashingSetup.h"

// #include "input-dependency/InputDependencyAnalysis.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
// #include "llvm/Support/TypeBuilder.h"
#include "llvm/IR/TypeBuilder.h"

using namespace llvm;
using namespace std;

static cl::opt<int> numHashVars("numHashVars", cl::desc("Number of distinct hash variables"), cl::Required);

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

    // Insert N hash variables
    errs() << "Going to insert " << numHashVars << " hash variables." << '\n';
    // Are there existing globals?
    Module::GlobalListType& globals = M.getGlobalList();
    errs() << globals.size() << '\n';

    // Try inserting one variable
    LLVMContext& ctx = M.getContext();
    Type *intType = llvm::TypeBuilder<int, false>::get(ctx);
    Value *global1 = M.getOrInsertGlobal("gHash1", intType);

    // global_hash1->setInitializer(c_ptr_null);
    Module::GlobalListType& globals2 = M.getGlobalList();
    errs() << globals2.size() << '\n';

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

/*
Garbage space
    // M.getOrInsertGlobal("gHash1", IntegerType::get(Type::getInt32Ty(ctx)), 0x99999);    
    // GlobalVariable* global_hash1 = new GlobalVariable(M, PointerTy_0, false, GlobalValue::CommonLinkage, 0, "hash1");
    // global_hash1.setAlignment(4);
    // ConstantPointerNull* c_ptr_null = ConstantPointerNull::get(PointerTy_0);

*/