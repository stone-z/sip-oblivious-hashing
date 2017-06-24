#include "ObliviousHashingSetup.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/TypeBuilder.h"

#include <iostream>
#include <unordered_set>

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
    insertSumHashFunction(M);
    insertSumOtherHashFunction(M);

    // Insert N hash variables - there appears to be a cap that LLVM enforces (12 for bubblesort)
    // insertHashVariables(numHashVars, M);
    std::unordered_set<std::string> hashVars = insertHashVariables(numHashVars, M);

    // Insert assertions at random points in the program
    insertAssertion(M);  // Add the assertEqual function to be called.
    // insertRandomly(M, numHashVars, 1);
    insertRandomly(M, hashVars, 1);
    return false;
}

// void ObliviousHashingSetupPass::insertRandomly(llvm::Module& M, int numberOfVariables, int numberOfChecks){
void ObliviousHashingSetupPass::insertRandomly(llvm::Module& M, std::unordered_set<std::string> listOfVariables, int numberOfChecks){
    // What should the probability be? N variables, B basic blocks, check every variable X times...? 

    dPrint("insertRandomly()");
    int blockCount = 0;
    // Get number of basic blocks in program
    for(Function& f: M){
        Function::BasicBlockListType& blocks = f.getBasicBlockList();
        errs() << "Number of basic blocks in function: " << f.getName() << ": " << blocks.size() << '\n';
        blockCount += blocks.size();
    }

    errs() << "Found " << std::to_string(blockCount) << " basic blocks." << '\n';
    int listSize = listOfVariables.size();

    // P(getsAssert) = X checks * N variables / B blocks
    double p = 1.0 - (1.0 * numberOfChecks * listSize / blockCount);
    // errs() << "numChecks " << std::to_string(numberOfChecks) << '\n';
    // errs() << "blockCount " << std::to_string(blockCount) << '\n';
    // errs() << "listSize " << std::to_string(listSize) << '\n';
    // errs() << "The magic number is " << p << '\n';

    // Shuffle the order of the globals
    // should change the return and parameter types to just be a vector. TODO
    std::vector<string> varVector;
    std::copy(listOfVariables.begin(), listOfVariables.end(), std::back_inserter(varVector));
    std::random_shuffle(varVector.begin(), varVector.end());

    // Maximum number of total checks = blockCount (i.e. every basic block does an assert)
    if(listOfVariables.size() <= blockCount){
        dPrint("Adding assertions");

        for(Function& f: M){
            Function::BasicBlockListType& blocks = f.getBasicBlockList();
            for(BasicBlock& b: blocks){
                double result = 1.0 * rand() / RAND_MAX;
                if(result < p){

                    // Would need to duplicate vector X times to check multiple times
                    if(!varVector.empty()){
                        errs() << std::to_string(result);
                        dPrint("- inserting assertion");
                        errs() << varVector.back() << '\n';

                        LLVMContext& ctx = M.getContext();
                        IRBuilder<> builder(ctx);
                        Constant* fhash1 = M.getOrInsertFunction("simpleSum", Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
                        Constant* fhash2 = M.getOrInsertFunction("simpleSumthingElse", Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
                        Constant* gvar_ptr = M.getOrInsertGlobal(varVector.back(), Type::getInt32Ty(ctx));

                        varVector.pop_back();  // remove the global
                    }
                }
            }
        }
    } else {
        // Can't instrument
        errs() << "Too many hash variables for program size." << '\n';
        exit(EXIT_FAILURE);
    }

}

void ObliviousHashingSetupPass::insertAssertion(llvm::Module& M){
    LLVMContext& context = M.getContext();
    Constant* assert = M.getOrInsertFunction("assertEqual", Type::getVoidTy(context), Type::getInt32PtrTy(context), Type::getInt32Ty(context), NULL);
}

void ObliviousHashingSetupPass::insertSumHashFunction(llvm::Module& M){
   // Function* f = M.getFunction("main");  // Just to test
     LLVMContext& context = M.getContext();
     Constant* simpleSum = M.getOrInsertFunction("simpleSum", Type::getVoidTy(context), Type::getInt32PtrTy(context), Type::getInt32Ty(context), NULL);
}

void ObliviousHashingSetupPass::insertSumOtherHashFunction(llvm::Module& M){
     //Function* f = M.getFunction("main");  // Just to test
     LLVMContext& context = M.getContext();
     Constant* simpleSumthingElse = M.getOrInsertFunction("simpleSumthingElse", Type::getVoidTy(context), Type::getInt32PtrTy(context), Type::getInt32Ty(context), NULL);
}

// This will probably need to return a list of handles to the globals
// Inserts an arbitrary number of global variables into the Module given
std::unordered_set<std::string> ObliviousHashingSetupPass::insertHashVariables(int numberOfVariables, llvm::Module& M){
    
    dPrint("Going to insert " + std::to_string(numberOfVariables) + " hash variables.");
    // Are there existing globals? If not, this might fail
    dPrint("Initial number of global variables:");
    Module::GlobalListType& globals = M.getGlobalList();
    errs() << globals.size() << '\n';
    
    std::unordered_set<std::string> names;

    LLVMContext& ctx = M.getContext(); 
    for(int i = 0; i < numberOfVariables; i++) { 
        std::string name = "gHash" + to_string(i);
        GlobalVariable* gvar = new GlobalVariable(M, Type::getInt32Ty(ctx), false, GlobalValue::CommonLinkage, ConstantInt::get(Type::getInt32Ty(ctx), 0), name);
        names.insert(name);
    }

    dPrint("Final number of global variables:");
    Module::GlobalListType& globals2 = M.getGlobalList();
    errs() << globals2.size() << '\n';
    
    return names;
}

// Print if the debug flag is set
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


        //for(int i = 0; i < numberOfVariables; i++) {
    //    LLVMContext& ctx = M.getContext();
    //    Type *intType = llvm::TypeBuilder<int, false>::get(ctx);
    //    // Could randomize/obscure the name to make it harder to find
    //    string hashVarName = "gHash" + std::to_string(i);
    //    Value *global = M.getOrInsertGlobal(hashVarName, intType);
    //    // Add to a list to return?
    //}



     //IRBuilder<> b(context);
     //Instruction* i = f->getEntryBlock().getFirstNonPHI();
     //b.SetInsertPoint(i);
     //ConstantInt* arg2 = ConstantInt::get(Type::getInt32Ty(context), 5);
     //Value* args[] = {/*arg1, arg2};
     //b.CreateCall(simpleSum, args);

     //IRBuilder<> b(context);
     //Instruction* i = f->getEntryBlock().getFirstNonPHI();
     //b.SetInsertPoint(i);
     //ConstantInt* arg2 = ConstantInt::get(Type::getInt32Ty(context), 5);
     //Value* args[] = {/*arg1, arg2};
     //b.CreateCall(simpleSum, args);

// This will probably need to return a list of handles to the globals
// Inserts an arbitrary number of global variables into the Module given
void ObliviousHashingSetupPass::insertHashVariables(int numberOfVariables, llvm::Module& M){
    
    dPrint("Going to insert " + std::to_string(numberOfVariables) + " hash variables.");
    // Are there existing globals? If not, this might fail
    dPrint("Initial number of global variables:");
    Module::GlobalListType& globals = M.getGlobalList();
    errs() << globals.size() << '\n';
    
    LLVMContext& ctx = M.getContext(); 
    for(int i = 0; i < numberOfVariables; i++) { 
       GlobalVariable* gvar = new GlobalVariable(M, Type::getInt32Ty(ctx), false, GlobalValue::CommonLinkage, ConstantInt::get(Type::getInt32Ty(ctx), 0), "gHash" + to_string(i));
    }

    dPrint("Final number of global variables:");
    Module::GlobalListType& globals2 = M.getGlobalList();
    errs() << globals2.size() << '\n';
 
}


*/
