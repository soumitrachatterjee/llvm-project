#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstVisitor.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"

using namespace llvm;

namespace {
class PointerAccessPrinter : public FunctionPass, public InstVisitor<PointerAccessPrinter> {
public:
  static char ID;
  PointerAccessPrinter() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    // Traverse the IR and identify instructions that involve pointer accesses
    visit(F);

    // Print the collected pointer addresses for the current function
    printFunctionPointerAddresses(F);

    return false;
  }

  // Instruction visitors to identify LoadInst and collect the pointer addresses
  void visitLoadInst(LoadInst &LI) {
    // Collect the pointer addresses for each function
    pointerAddresses.push_back(LI.getPointerOperand());
  }

  // Function to print the collected pointer addresses for a function
  void printFunctionPointerAddresses(Function &F) {
    errs() << "\t" << F.getName() << "(), ";
    for (Value* pointerAddr : pointerAddresses) {
      errs() << pointerAddr << ", ";
    }
    errs() << "\n\n";
    pointerAddresses.clear(); // Clear the collected addresses for the next function
  }

private:
  std::vector<Value*> pointerAddresses;
};
} // namespace

char PointerAccessPrinter::ID = 0;

static RegisterPass<PointerAccessPrinter> X("pointer-access-printer", "Pointer Access Printer Pass", false, false);

