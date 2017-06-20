#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

#include "ObliviousHashingTransformation.h"
#include "input-dependency/InputDependencyAnalysis.h"

using namespace llvm;
using namespace std;

namespace {

   char ObliviousHashingTransformationPass::ID = 0;

   void ObliviousHashingTransformationPass::getAnalysisUsage(AnalysisUsage& AU) const {
      AU.setPreservesAll();
      AU.addRequired<input_dependency::InputDependencyAnalysis>();
   }

   bool ObliviousHashingTransformationPass::runOnModule(llvm::Module& M) {
      AnalysisUsage analysis;
      getAnalysisUsage(analysis);
      const auto& input_dependency_info = getAnalysis<input_dependency::InputDependencyAnalysis>();
      for(Function& f: M) {
         errs() << "I am " << f.getName() << '\n';
         Function::BasicBlockListType& bbs = f.getBasicBlockList();
         unsigned counter = 0;
         for(BasicBlock& bb: bbs) {
            //errs() << "I am block " << counter << '\n';
            counter++;
            for(Instruction& i: bb) {
               if(input_dependency_info.isInputDependent(&i)) {
                  errs() << "I am input dependent" << '\n';
               }
               //errs() << "I am instruction\n";
               i.dump();
            }
         }
      }
      return false;
   }

}

static llvm::RegisterPass<ObliviousHashingTransformationPass> X("oh-transform-pass", "Inserts hash values into hash variables");



