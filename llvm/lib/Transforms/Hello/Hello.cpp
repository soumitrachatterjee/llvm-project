#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfoMetadata.h"

using namespace llvm;
using namespace std;

#define DEBUG_TYPE "cotr"

STATISTIC(CotrCounter, "Counts number of constructors invoked implicitly");

namespace {
  struct Hello : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    Hello() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
      ++CotrCounter;
      
      SmallVector<std::pair<unsigned, MDNode *>, 4> MDs;
F.getAllMetadata(MDs);
for (auto &MD : MDs) {
  if (MDNode *N = MD.second) {
    if (auto *subProgram = dyn_cast<DISubprogram>(N)) {
      errs() << subProgram->getLine();
    }
  }
}

	
      
      
      StringRef functionName = F.getName();
      if (functionName.startswith("_ZN")){	
	StringRef temp;
	bool flag = true;
	for (BasicBlock &BB : F) {
      for (Instruction &I : BB) {
      
      
      
        if (DILocation *Loc = I.getDebugLoc()) {
          // Check if debugging information is available for the instruction.
          unsigned Line = Loc->getLine();
          if (!Loc->getFilename().empty()) {
            StringRef Filename = Loc->getFilename();
            temp = Filename;
            // Now you can use the Filename as needed.
            // For example, you can print it or perform some analysis based on it.
            if(flag){
            	errs() << Line << ": " <<"Source Filename: " << Filename << "\n";
            }
            else{
            	if(temp != Filename){
            		errs() << "Source Filename: " << Filename << "\n";
            		flag = true;
            	}
            }
            flag = false;
          }
        }
      }
     }
      	errs() << "Invoked: ";
      	errs().write_escaped(F.getName()) << '\n';
      }
      return false;
    }
  };
}

char Hello::ID = 0;
static RegisterPass<Hello> X("cotr", "Implicitly Called Constructors");
