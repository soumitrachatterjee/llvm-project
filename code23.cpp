#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/MemorySSA.h"
#include <map>

using namespace llvm;

namespace {
struct MemoryAccessPattern : public FunctionPass {
    static char ID;
    MemoryAccessPattern() : FunctionPass(ID) {}

    struct MemoryAccessInfo {
        std::string location;
        unsigned size;
        unsigned count;
    };

    std::map<std::string, MemoryAccessInfo> accessMap;

    bool runOnFunction(Function &F) override {
        MemorySSA &MSSA = getAnalysis<MemorySSAWrapperPass>().getMSSA();
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (MemoryUseOrDef *MUD = MSSA.getMemoryAccess(&I)) {
                    std::string location = getLocationString(MUD);
                    unsigned size = getTypeSize(I.getType());

                    auto it = accessMap.find(location);
                    if (it == accessMap.end()) {
                        accessMap[location] = {location, size, 1};
                    } else {
                        it->second.size += size;
                        it->second.count++;
                    }
                }
            }
        }

        printMemoryAccessInfo(F);
        return false;
    }

    std::string getLocationString(MemoryUseOrDef *MUD) {
        if (MemoryDef *MD = dyn_cast<MemoryDef>(MUD)) {
            if (Instruction *MI = MD->getMemoryInst()) {
                Value *ptr = MI->getOperand(0);
                return ptr->getName().str();
            }
        }
        return "Unknown Location";
    }

    unsigned getTypeSize(Type *Ty) {
        return Ty->getPrimitiveSizeInBits() / 8;
    }

    void printMemoryAccessInfo(const Function &F) {
        errs() << F.getParent()->getSourceFileName() << ":" << F.getName() << "()\n";
        for (const auto &entry : accessMap) {
            const MemoryAccessInfo &info = entry.second;
            errs() << "  " << info.location << ": " << info.size << " bytes (" << info.count << " times)\n";
        }
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
        AU.addRequired<MemorySSAWrapperPass>();
        AU.setPreservesAll();
    }
};
} // namespace

char MemoryAccessPattern::ID = 0;

static RegisterPass<MemoryAccessPattern> X("memory-access-pattern", "Memory Access Pattern Pass");
