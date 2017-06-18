#include "ObliviousHashingSetup.h"

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
#include "llvm/IR/TypeBuilder.h"

#define PrintDebug

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
    dPrint("Pass is running");
    
    // Insert at least 2 different hash functions


    // Insert N hash variables - there appears to be a cap that LLVM enforces (12 for bubblesort)
    insertHashFunctions(numHashVars, M);

    // Insert assertions at random points in the program
    return false;
}

// This will probably need to return a list of handles to the globals
void ObliviousHashingSetupPass::insertHashFunctions(int numberOfVariables, llvm::Module& M){
    dPrint("Going to insert " + std::to_string(numberOfVariables) + " hash variables.");
    
    // Are there existing globals? If not, this might fail
    dPrint("Initial number of global variables:");
    Module::GlobalListType& globals = M.getGlobalList();
    errs() << globals.size() << '\n';

    for(int i = 0; i < numberOfVariables; i++){
        LLVMContext& ctx = M.getContext();
        Type *intType = llvm::TypeBuilder<int, false>::get(ctx);
        string hashVarName = "gHash" + std::to_string(i);
        Value *global = M.getOrInsertGlobal(hashVarName, intType);
    }

    dPrint("Final number of global variables:");
    Module::GlobalListType& globals2 = M.getGlobalList();
    errs() << globals2.size() << '\n';
}

void ObliviousHashingSetupPass::dPrint(string message){
    #ifdef PrintDebug
    errs() << message << '\n';
    #endif
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
    // global_hash1->setInitializer(c_ptr_null);

*/