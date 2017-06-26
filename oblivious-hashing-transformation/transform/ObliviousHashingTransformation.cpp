#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <time.h>
#include <cstring>

#include "ObliviousHashingTransformation.h"
#include "input-dependency/InputDependencyAnalysis.h"

using namespace llvm;
using namespace std;

static cl::opt<int> numHashVars("numHashVars", cl::desc("Number of distinct hash variables"), cl::Required);
static cl::list<string> hfParam("hf", cl::desc("Hash function name"), cl::OneOrMore);

namespace {

   char ObliviousHashingTransformationPass::ID = 0;

   void ObliviousHashingTransformationPass::getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
      AU.addRequired<input_dependency::InputDependencyAnalysis>();
   }

   void ObliviousHashingTransformationPass::insertInstrumentation(Module& M, Instruction& i) {
        LLVMContext& ctx = M.getContext();
        IRBuilder<> builder(ctx);
        Constant* fhash1 = M.getOrInsertFunction(hfParam[0], Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
        Constant* fhash2 = M.getOrInsertFunction(hfParam[1], Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
        
        unsigned global = rand() % numHashVars;
        unsigned hash_func = rand() % 2;
        errs() << "Let's use global " << global << " and function " << hash_func << ".\n";
        Constant* gvar_ptr = M.getOrInsertGlobal("gHash" + to_string(global), Type::getInt32Ty(ctx));
        Constant* func;
        if(hash_func == 0) {
           func = fhash1;
        } else {
           func = fhash2;
        }
        
        if(isa<LoadInst>(i) && (*(i.getOperand(0))).getType() == Type::getInt32PtrTy(ctx)) {
           builder.SetInsertPoint(i.getNextNode());
           Value* args_to_hash[] = {gvar_ptr, &i};  
           builder.CreateCall(func, args_to_hash);
        } else if(isa<StoreInst>(i) && (*(i.getOperand(0))).getType() == Type::getInt32Ty(ctx)) {
           Value* op = i.getOperand(0);
           Value* args_to_hash[] = {gvar_ptr, op}; 
           builder.SetInsertPoint(i.getNextNode());
           builder.CreateCall(func, args_to_hash);
        } else if(isa<BranchInst>(i) && cast<BranchInst>(i).isConditional()) {
           Instruction* cond = cast<Instruction>(cast<BranchInst>(i).getCondition());
           CmpInst::Predicate pred = cast<ICmpInst>(*cond).getUnsignedPredicate();
           Value* op1 = cond->getOperand(0);
           Value* op2 = cond->getOperand(1);
           Constant* predicate = ConstantInt::get(Type::getInt32Ty(ctx), pred);
           Value* args_to_hash[] = {gvar_ptr, predicate}; 
           builder.SetInsertPoint(&i);
           builder.CreateCall(func, args_to_hash);
           if(isa<LoadInst>(*op1)) {
              args_to_hash[1] = op1;
              builder.CreateCall(func, args_to_hash);
           }     
           if(isa<LoadInst>(*op2)) {
              args_to_hash[1] = op2;
              builder.CreateCall(func, args_to_hash);
           }
        } else if(i.isBinaryOp()) {
             Value* op1 = i.getOperand(0);
             Value* op2 = i.getOperand(1);
             Value* args_to_hash[] = {gvar_ptr, op1}; 
             builder.SetInsertPoint(i.getNextNode());
             builder.CreateCall(func, args_to_hash);
             args_to_hash[1] = op2;
             builder.CreateCall(func, args_to_hash);
        }
   }

   void ObliviousHashingTransformationPass::insertHashVariables(llvm::Module& M) {
      LLVMContext& ctx = M.getContext();
      for(int i = 0; i < numHashVars; i++) {
        std::string name = "gHash" + to_string(i);
        GlobalVariable* gvar = new GlobalVariable(M, Type::getInt32Ty(ctx), false, GlobalValue::CommonLinkage, ConstantInt::get(Type::getInt32Ty(ctx), 0), name); 
      }
   }

   bool ObliviousHashingTransformationPass::runOnModule(llvm::Module& M) {
      srand(time(NULL));

      AnalysisUsage analysis;
      getAnalysisUsage(analysis);
      const auto& input_dependency_info = getAnalysis<input_dependency::InputDependencyAnalysis>();

      insertHashVariables(M);

      for(Function& f: M) {
         Function::BasicBlockListType& bbs = f.getBasicBlockList();
         for(BasicBlock& bb: bbs) {
            for(Instruction& i: bb) {
               if(!input_dependency_info.isInputDependent(&i)) {
                  if(isa<LoadInst>(i) || isa<StoreInst>(i) || isa<BranchInst>(i) || i.isBinaryOp()) {
                        insertInstrumentation(M, i);
                  }
               }
            }
         }
      }
      return false;
   }

}

static llvm::RegisterPass<ObliviousHashingTransformationPass> X("oh-transform-pass", "Inserts hash values into hash variables");



