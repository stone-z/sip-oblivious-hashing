#include "llvm/IR/Module.h"
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

   bool ObliviousHashingTransformationPass::runOnModule(llvm::Module& M) {
      srand(time(NULL));

      AnalysisUsage analysis;
      getAnalysisUsage(analysis);
      const auto& input_dependency_info = getAnalysis<input_dependency::InputDependencyAnalysis>();

      LLVMContext& ctx = M.getContext();
      IRBuilder<> builder(ctx);

      for(Function& f: M) {
         //errs() << "I am " << f.getName() << '\n';
         Function::BasicBlockListType& bbs = f.getBasicBlockList();
         unsigned counter = 0;
         for(BasicBlock& bb: bbs) {
            //errs() << "I am block " << counter << '\n';
            counter++;
            for(Instruction& i: bb) {
               if(!input_dependency_info.isInputDependent(&i)) {
                  //errs() << "I am not input dependent" << '\n';
                  //i.dump();
                  const char* name = Instruction::getOpcodeName(i.getOpcode());
                  unsigned global;
                  unsigned hash_func;
                  Constant* fhash1 = M.getOrInsertFunction(hfParam[0], Type::getVoidTy(ctx), /*Type::getInt32PtrTy(ctx),*/ Type::getInt32Ty(ctx), NULL);
                  Constant* fhash2 = M.getOrInsertFunction(hfParam[1], Type::getVoidTy(ctx), /*Type::getInt32PtrTy(ctx),*/ Type::getInt32Ty(ctx), NULL);
                  if(isa<LoadInst>(i)) {
                     errs() << "I am load\n";
                     i.dump();
                     global = rand() % numHashVars;
                     hash_func = rand() % 2;
                     errs() << "Let's use global " << global << " and function " << hash_func << '\n';
                     GlobalVariable* gvar_ptr = M.getNamedGlobal("gHash" + to_string(global));
                     //Value* load_arg = cast<LoadInst>(i).getOperand(0);
                     Value* args_to_hash[] = {/*gvar_ptr,*/ /*load_arg*/ &i}; 
                     builder.SetInsertPoint(i.getNextNode());
                     if(hash_func == 0) {
                        builder.CreateCall(fhash1, args_to_hash);
                     } else {
                        builder.CreateCall(fhash2, args_to_hash);
                     }
                  }
               }
               //errs() << "I am instruction\n";
            }
         }
      }
      return false;
   }

}

static llvm::RegisterPass<ObliviousHashingTransformationPass> X("oh-transform-pass", "Inserts hash values into hash variables");



