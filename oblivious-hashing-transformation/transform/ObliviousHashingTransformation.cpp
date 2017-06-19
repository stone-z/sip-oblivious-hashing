#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "ObliviousHashingTransformation.h"

using namespace llvm;
using namespace std;

namespace {

   char ObliviousHashingTransformationPass::ID = 0;

   bool ObliviousHashingTransformationPass::runOnModule(llvm::Module& M) {
      errs() << M.getName() << '\n';
      return false;
   }

}

static llvm::RegisterPass<ObliviousHashingTransformationPass> X("oh-transform-pass", "Inserts hash values into hash variables");



