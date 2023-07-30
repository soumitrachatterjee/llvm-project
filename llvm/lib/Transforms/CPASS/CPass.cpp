#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<std::string> CompileCommand("compile-command", cl::desc("Compilation Command"), cl::value_desc("compile_command"));

namespace {
  struct CPASS : public ModulePass {
    static char ID;
    CPASS() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) override {
      if (CompileCommand.empty()) {
        errs() << "Error: Compile command not provided. Use -compile-command=<command>\n";
        return false;
      }

      LLVMContext &Context = M.getContext();

      // Create the mangled name for the global variable
      std::string sourceFileName;
      std::string command;
      int found=0;
      for(int i=0;i<CompileCommand.size();i++) {
        if(found==1) {
          command.push_back(CompileCommand[i]);
        } else if(CompileCommand[i]==',') {
          found=1;
        } else {
          sourceFileName.push_back(CompileCommand[i]);
        }
      }
      std::string mangledName = "__cli_" + sourceFileName;
      for (char& c : mangledName) {
        if (!isalnum(c))
          c = '_'; // Replace non-alphanumeric characters with '_'
      }

      // Create the global variable of type 'const char*' with the mangled name
      Constant* strConstant = ConstantDataArray::getString(Context, command, true);
      GlobalVariable* globalVar = new GlobalVariable(
          M, strConstant->getType(), true, GlobalValue::LinkageTypes::ExternalLinkage,
          strConstant, mangledName);

      // Set the global variable as a constant (read-only)
      globalVar->setConstant(true);

      return true; // Module has been modified
    }
  };
}

char CPASS::ID = 0;
static RegisterPass<CPASS> X("cpass", "My custom LLVM pass", false, false);