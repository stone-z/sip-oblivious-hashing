#include "llvm/Pass.h"

namespace {

class ObliviousHashingTransformationPass : public llvm::ModulePass {
    public:
        static char ID;
        ObliviousHashingTransformationPass(): llvm::ModulePass(ID) {}
        bool runOnModule(llvm::Module& M) override;
     // virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

}

