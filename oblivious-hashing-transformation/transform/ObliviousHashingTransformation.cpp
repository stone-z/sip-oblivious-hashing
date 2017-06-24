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
        errs() << "Let's use global " << global << " and function " << hash_func << '\n';
        Constant* gvar_ptr = M.getOrInsertGlobal("gHash" + to_string(global), Type::getInt32Ty(ctx));
        Constant* func;
        if(hash_func == 0) {
           func = fhash1;
        } else {
           func = fhash2;
        }
        
        if(isa<LoadInst>(i) && (*(i.getOperand(0))).getType() == Type::getInt32PtrTy(ctx)) {
           errs() << "I am load\n";
           i.dump();
           builder.SetInsertPoint(i.getNextNode());
           Value* args_to_hash[] = {gvar_ptr, &i};  
           builder.CreateCall(func, args_to_hash);
        } else if(isa<StoreInst>(i) && (*(i.getOperand(0))).getType() == Type::getInt32Ty(ctx)) {
           errs() << "I am store\n";
           i.dump();
           Value* op = i.getOperand(0);
           Value* args_to_hash[] = {gvar_ptr, op}; 
           builder.SetInsertPoint(i.getNextNode());
           builder.CreateCall(func, args_to_hash);
        } else if(isa<BranchInst>(i) && cast<BranchInst>(i).isConditional()) {
           Instruction* cond = cast<Instruction>(cast<BranchInst>(i).getCondition());
           CmpInst::Predicate pred = cast<ICmpInst>(*cond).getUnsignedPredicate();
           cond->dump();
           errs() << "Predicate " << pred << '\n';
           Value* op1 = cond->getOperand(0);
           op1->dump();
           Value* op2 = cond->getOperand(1);
           op2->dump();
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
             errs() << "I am binary\n";
             i.dump();
             i.getOperand(0)->dump();
             i.getOperand(1)->dump();
             Value* op1 = i.getOperand(0);
             Value* op2 = i.getOperand(1);
             Value* args_to_hash[] = {gvar_ptr, op1}; 
             builder.SetInsertPoint(i.getNextNode());
             builder.CreateCall(func, args_to_hash);
             args_to_hash[1] = op2;
             builder.CreateCall(func, args_to_hash);
        }
   }

   bool ObliviousHashingTransformationPass::runOnModule(llvm::Module& M) {
      srand(time(NULL));

      AnalysisUsage analysis;
      getAnalysisUsage(analysis);
      const auto& input_dependency_info = getAnalysis<input_dependency::InputDependencyAnalysis>();

      LLVMContext& ctx = M.getContext();
      IRBuilder<> builder(ctx);
      Constant* fhash1 = M.getOrInsertFunction(hfParam[0], Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
      Constant* fhash2 = M.getOrInsertFunction(hfParam[1], Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
      for(Function& f: M) {
         Function::BasicBlockListType& bbs = f.getBasicBlockList();
         for(BasicBlock& bb: bbs) {
            for(Instruction& i: bb) {
               if(!input_dependency_info.isInputDependent(&i)) {
                  //errs() << "I am not input dependent" << '\n';
                  //i.dump();
               //   const char* name = Instruction::getOpcodeName(i.getOpcode());
               //   unsigned global;
               //   unsigned hash_func;
                  if(isa<LoadInst>(i) || isa<StoreInst>(i) || isa<BranchInst>(i) || i.isBinaryOp()) {
               //      if(load_arg->getType() == Type::getInt32PtrTy(ctx)) {
                        insertInstrumentation(M, i);
               }      /*   errs() << "I am load\n";
                        i.dump();
                        global = rand() % numHashVars;
                        hash_func = rand() % 2;
                        errs() << "Let's use global " << global << " and function " << hash_func << '\n';
                        Constant* gvar_ptr = M.getOrInsertGlobal("gHash" + to_string(global), Type::getInt32Ty(ctx));
                        Value* args_to_hash[] = {gvar_ptr, &i}; 
                        builder.SetInsertPoint(i.getNextNode());
                        Constant* func;
                        if(hash_func == 0) {
                           func = fhash1;
                        } else {
                           func = fhash2;
                        }
                        builder.CreateCall(func, args_to_hash); */
               //      } 
               //   } else if(isa<StoreInst>(i)) {
                     /*errs() << "I am store\n";
                     i.dump();
                     global = rand() % numHashVars;
                     hash_func = rand() % 2;
                     errs() << "Let's use global " << global << " and function " << hash_func << '\n';
                     Constant* gvar_ptr = M.getOrInsertGlobal("gHash" + to_string(global), Type::getInt32Ty(ctx));
                     Value* op1 = i.getOperand(0);
                     if(op1->getType() == Type::getInt32Ty(ctx)) {
                        errs() << "operand 1 ";
                        op1->dump();
                        Value* op2 = i.getOperand(1);
                        errs() << "operand 2 ";
                        op2->dump();
                        Value* args_to_hash[] = {gvar_ptr, op1}; 
                        builder.SetInsertPoint(i.getNextNode());
                        if(hash_func == 0) {
                           builder.CreateCall(fhash1, args_to_hash);
                           hash1counter++;
                        } else {
                           builder.CreateCall(fhash2, args_to_hash);
                           hash2counter++;
                        }
                     }*/
                 // } else if(isa<BranchInst>(i)) {
                       /*if(cast<BranchInst>(i).isConditional()) {
                          errs() << "I am a branch\n";
                          i.dump();
                          global = rand() % numHashVars;
                          hash_func = rand() % 2;
                          errs() << "Let's use global " << global << " and function " << hash_func << '\n';
                          Instruction* cond = cast<Instruction>(cast<BranchInst>(i).getCondition());
                          CmpInst::Predicate pred = cast<ICmpInst>(*cond).getUnsignedPredicate();
                          cond->dump();
                          errs() << "Predicate " << pred << '\n';
                          Value* op1 = cond->getOperand(0);
                          op1->dump();
                          Value* op2 = cond->getOperand(1);
                          op2->dump();
                          
                          Constant* gvar_ptr = M.getOrInsertGlobal("gHash" + to_string(global), Type::getInt32Ty(ctx));
                          Constant* predicate = ConstantInt::get(Type::getInt32Ty(ctx), pred);
                          Value* args_to_hash[] = {gvar_ptr, predicate}; 
                          builder.SetInsertPoint(&i);
                          Constant* func; 
                          if(hash_func == 0) {
                             func = fhash1;
                             builder.CreateCall(func, args_to_hash);
                          } else {
                             func = fhash2;
                             builder.CreateCall(func, args_to_hash);
                          }
                          if(isa<LoadInst>(*op1)) {
                             args_to_hash[1] = op1;
                             builder.CreateCall(func, args_to_hash);
                          }	
                          if(isa<LoadInst>(*op2)) {
                             args_to_hash[1] = op2;
                             builder.CreateCall(func, args_to_hash);
		          }
                       }*/
                 /* } else if(i.isBinaryOp()) {
                     errs() << "I am binary\n";
                     i.dump();
                     i.getOperand(0)->dump();
                     i.getOperand(1)->dump();
                     global = rand() % numHashVars;
                     hash_func = rand() % 2;
                     errs() << "Let's use global " << global << " and function " << hash_func << '\n';
                     Constant* gvar_ptr = M.getOrInsertGlobal("gHash" + to_string(global), Type::getInt32Ty(ctx));
                     Value* op1 = i.getOperand(0);
                     Value* op2 = i.getOperand(1);
                     Value* args_to_hash[] = {gvar_ptr, op1}; 
                     builder.SetInsertPoint(i.getNextNode());
                     Constant* func; 
                     if(hash_func == 0) {
                        func = fhash1;
                     } else {
                        func = fhash2;
                     }
                     builder.CreateCall(func, args_to_hash);
                     args_to_hash[1] = op2;
                     builder.CreateCall(func, args_to_hash);
                  }*/
               }
            }
         }
      }
      return false;
   }

}

static llvm::RegisterPass<ObliviousHashingTransformationPass> X("oh-transform-pass", "Inserts hash values into hash variables");



