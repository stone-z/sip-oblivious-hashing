#include "ObliviousHashingInject.h"

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

#define PrintDebug

#include <iostream>
#include <unordered_set>

using namespace llvm;
using namespace std;

static cl::opt<int> numHashVars("numHashVars", cl::desc("Number of distinct hash variables"), cl::Required);

namespace {

char ObliviousHashingInjectPass::ID = 0;


bool ObliviousHashingInjectPass::runOnModule(llvm::Module& M)
{
    dPrint("Pass is running");
    
    // Insert assertions at random points in the program
    insertAssertion(M);  // Add the assertEqual function to be called.
    insertRandomly(M, 1);

    dPrint("End of inject pass.");

    return true;
}

void ObliviousHashingInjectPass::insertRandomly(llvm::Module& M, int numberOfChecks){

    dPrint("Counting basic blocks.");
    int blockCount = 0;
    // Get number of basic blocks in program
    for(Function& f: M){
        Function::BasicBlockListType& blocks = f.getBasicBlockList();
        blockCount += blocks.size();
    }

    errs() << "Found " << std::to_string(blockCount) << " basic blocks." << '\n';

    // P(getsAssert) = X checks * N variables / B blocks
    double p = 1.0 - (1.0 * numberOfChecks * numHashVars / blockCount);
    // errs() << "numChecks " << std::to_string(numberOfChecks) << '\n';
    // errs() << "blockCount " << std::to_string(blockCount) << '\n';
    // errs() << "listSize " << std::to_string(listSize) << '\n';
    //errs() << "The magic number is " << p << '\n';

    // Shuffle the order of the globals
    std::vector<string> varVector;
    for(unsigned i = 0; i < numHashVars; i++) {
       varVector.push_back("gHash" + std::to_string(i));
    }
    std::random_shuffle(varVector.begin(), varVector.end());
    // Maximum number of total checks = blockCount (i.e. every basic block does an assert)
    if(numHashVars <= blockCount){
        dPrint("Adding assertions");

        for(Function& f: M){
            Function::BasicBlockListType& blocks = f.getBasicBlockList();
            for(BasicBlock& b: blocks){
                double result = 1.0 * rand() / RAND_MAX;
                if(result < p){
               
                    // Would need to duplicate vector X times to check multiple times
                    if(!varVector.empty()){
                        errs() << "  - Inserting: " << varVector.back() << '\n';

                        LLVMContext& ctx = M.getContext();
                        IRBuilder<> builder(ctx);                        
                  
                        // Set insertion location
                        Instruction& i = b.front();
                        builder.SetInsertPoint(&i);
                        
                        // Collect function and arguments
                        Constant* assertion = M.getOrInsertFunction("assertEqual", Type::getInt32Ty(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
                        Constant* globalVar = M.getOrInsertGlobal(varVector.back(), Type::getInt32Ty(ctx));
                        Constant* expectedHash = ConstantInt::get(Type::getInt32Ty(ctx), 0xdeadbeef);
                        Value* assertArgs[] = {globalVar, expectedHash};
                        
                        builder.CreateCall(assertion, assertArgs);  // Insert the call

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

void ObliviousHashingInjectPass::insertAssertion(llvm::Module& M){
    LLVMContext& context = M.getContext();
    Constant* assert = M.getOrInsertFunction("assertEqual", Type::getVoidTy(context), Type::getInt32PtrTy(context), Type::getInt32Ty(context), NULL);
}

// Print if the debug flag is set
void ObliviousHashingInjectPass::dPrint(string message){
    #ifdef PrintDebug
    errs() << message << '\n';
    #endif
}


static llvm::RegisterPass<ObliviousHashingInjectPass> X("oh-inject-pass","Prepares a program for oblivious hashing instrumentation.");

}
