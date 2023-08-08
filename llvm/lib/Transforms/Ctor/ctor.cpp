#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
  struct ImplicitConstructorPass : public FunctionPass {
    static char ID;
    ImplicitConstructorPass() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      for (BasicBlock &BB : F) {
        for (Instruction &I : BB) {
          if (auto *CI = dyn_cast<CallInst>(&I)) {
            Function *calledFunction = CI->getCalledFunction();
            if (calledFunction) {
              StringRef functionName = calledFunction->getName();
              if (functionName.startswith("_ZN")) {
                // Assume it's a constructor call
                // Get the source location information
                DebugLoc DL = CI->getDebugLoc();
                if (DL) {
                  StringRef fileName = DL->getFilename();
                  int lineNumber = DL->getLine();
                  int columnNumber = DL->getColumn();

                  errs() << "Implicit constructor invocation in function '"
                         << F.getName() << "' at " << fileName << ":" << lineNumber
                         << ":" << columnNumber << "\n";
                }
              }
            }
          }
        }
      }
      return false;
    }
  };
}

char ImplicitConstructorPass::ID = 0;
static RegisterPass<ImplicitConstructorPass> X("implicit-constructors", "Identify Implicit Constructor Invocations",false,false);

