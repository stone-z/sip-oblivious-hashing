#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"

namespace {

class ObliviousHashingTransformationPass : public llvm::ModulePass {
    public:
        static char ID;
        ObliviousHashingTransformationPass(): llvm::ModulePass(ID) {}
        bool runOnModule(llvm::Module& M) override;
        void insertInstrumentation(llvm::Module& M, llvm::Instruction& i); 
        virtual void getAnalysisUsage(llvm::AnalysisUsage& AU) const override;
};

}

