#include "llvm/include/llvm/IR/Module.h"
#include "llvm/include/llvm/IR/LLVMContext.h"
#include "llvm/include/llvm/IR/GlobalVariable.h"
#include "llvm/include/llvm/IR/Constant.h"
#include "llvm/include/llvm/Pass.h"
#include "llvm/include/llvm/Support/raw_ostream.h"
#include "llvm/include/llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<std::string> SourceFileName("source-file-name", cl::desc("Source file name"), cl::value_desc("filename"));

namespace {
  struct MyPluginPass : public ModulePass {
    static char ID;
    MyPluginPass() : ModulePass(ID) {}

    virtual bool runOnModule(Module &M) override {
      if (SourceFileName.empty()) {
        errs() << "Error: Source file name not provided. Use -source-file-name=<filename>\n";
        return false;
      }

      LLVMContext &Context = M.getContext();

      // Create the mangled name for the global variable
      std::string mangledName = "__cli_" + SourceFileName;
      for (char& c : mangledName) {
        if (!isalnum(c))
          c = '_'; // Replace non-alphanumeric characters with '_'
      }

      // Create the global variable of type 'const char*' with the mangled name
      Constant* strConstant = ConstantDataArray::getString(Context, SourceFileName, true);
      GlobalVariable* globalVar = new GlobalVariable(
          M, strConstant->getType(), true, GlobalValue::LinkageTypes::ExternalLinkage,
          strConstant, mangledName);

      // Set the global variable as a constant (read-only)
      globalVar->setConstant(true);

      return true; // Module has been modified
    }
  };
}

char MyPluginPass::ID = 0;
static RegisterPass<MyPluginPass> X("my-plugin-pass", "My custom LLVM pass", false, false);
