//===- PassManager.cpp - Infrastructure for managing & running IR passes --===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "llvm/IR/PassManager.h"
#include "llvm/IR/PassManagerImpl.h"
#include <optional>
#define custom_variable "isFragile"
#include "llvm/IR/Instructions.h"

#include "llvm/IR/Attributes.h"

#include "llvm/ADT/StringRef.h"

#include "llvm/IR/Module.h"

#include <llvm/IR/DebugLoc.h>

#include <llvm/IR/DebugInfoMetadata.h>

static llvm::Attribute getFragileAttr(llvm::LLVMContext &Ctx) {

  return llvm::Attribute::get(Ctx, custom_variable, "true");
}

struct FragileCluster {

  int start;

  int end;
};

struct FragileCluster s[6] = {{1, 4},   {9, 10},  {12, 13},
                              {17, 19}, {27, 30}, {37, 40}};

using namespace llvm;
void markFragile(Module &M) {
  for (Function &F : M) {
    int flag = 0;
    for (BasicBlock &BB : F) {
      int Line = 0, Line1 = 0;
      for (Instruction &I : BB) {
        if (DILocation *Loc = I.getDebugLoc()) {
          Line = Loc->getLine();
          break;
        }
      }
      Instruction &I1 = *(BB.getTerminator());

      if (DILocation *Loc = I1.getDebugLoc()) {
        Line1 = Loc->getLine();
      }

      for (int i = 0; i < 6; i++) {
        if (((Line >= s[i].start and Line <= s[i].end) or
             (Line1 >= s[i].start and Line1 <= s[i].end)) or
            ((s[i].start >= Line and s[i].start <= Line1) or
             (s[i].end >= Line and s[i].end <= Line1))) {

          flag = 1;
          // errs() << F.getName() << "\n"
          //   << "Lines- " << Line << ":" << Line1
          //   << " overlaps with given range : " << s[i].start << " "
          //   << s[i].end << "\n";
        }
      }
    }

    if (flag == 1) {
      F.addFnAttr(getFragileAttr(F.getContext()));
    }
  }
}
void displayFragile(Module &M) {
  errs() << "-------------Fragile Functions---------------"
         << "\n";

  for (Function &F : M)

  {
    if (F.hasFnAttribute(custom_variable)) {
      errs() << "Function Name: " << F.getName() << "\n";
      errs() << "Signature: " << *F.getReturnType() << " " << F.getName()
             << "(";
      int first = 0;
      for (llvm::Argument &Arg : F.args()) {
        if (first == 0) {

          outs() << *Arg.getType() << Arg.getName();
          first = 1;
        } else {
          outs() << " ," << *Arg.getType() << Arg.getName();
        }
      }
      errs() << ")"
             << "\n";
      int Line = 0, Line1 = 0;
      BasicBlock &firstBB = F.front();
      BasicBlock &lastBB = F.back();
      for (Instruction &I : firstBB) {
        if (DILocation *Loc = I.getDebugLoc()) {
          Line = Loc->getLine();
          break;
        }
      }
      Instruction &I1 = *(lastBB.getTerminator());
      if (DILocation *Loc = I1.getDebugLoc()) {
        Line1 = Loc->getLine();
      }
      errs() << "Size of code: " << Line1 - Line + 1 << " Lines of code"
             << "\n";

      if (DISubprogram *SP = F.getSubprogram()) {

        outs() << "Source File: " << SP->getFilename() << "\n";
      }
      errs() << "---------------------------------------------"
             << "\n";
    }
  }
}

namespace llvm {
// Explicit template instantiations and specialization defininitions for core
// template typedefs.
template class AllAnalysesOn<Module>;
template class AllAnalysesOn<Function>;
template class PassManager<Module>;
template class PassManager<Function>;
template class AnalysisManager<Module>;
template class AnalysisManager<Function>;
template class InnerAnalysisManagerProxy<FunctionAnalysisManager, Module>;
template class OuterAnalysisManagerProxy<ModuleAnalysisManager, Function>;

template <>
bool FunctionAnalysisManagerModuleProxy::Result::invalidate(
    Module &M, const PreservedAnalyses &PA,
    ModuleAnalysisManager::Invalidator &Inv) {
  // If literally everything is preserved, we're done.
  if (PA.areAllPreserved())
    return false; // This is still a valid proxy.

  // If this proxy isn't marked as preserved, then even if the result remains
  // valid, the key itself may no longer be valid, so we clear everything.
  //
  // Note that in order to preserve this proxy, a module pass must ensure that
  // the FAM has been completely updated to handle the deletion of functions.
  // Specifically, any FAM-cached results for those functions need to have been
  // forcibly cleared. When preserved, this proxy will only invalidate results
  // cached on functions *still in the module* at the end of the module pass.
  auto PAC = PA.getChecker<FunctionAnalysisManagerModuleProxy>();
  if (!PAC.preserved() && !PAC.preservedSet<AllAnalysesOn<Module>>()) {
    InnerAM->clear();
    return true;
  }

  // Directly check if the relevant set is preserved.
  bool AreFunctionAnalysesPreserved =
      PA.allAnalysesInSetPreserved<AllAnalysesOn<Function>>();

  // Now walk all the functions to see if any inner analysis invalidation is
  // necessary.
  for (Function &F : M) {
    std::optional<PreservedAnalyses> FunctionPA;

    // Check to see whether the preserved set needs to be pruned based on
    // module-level analysis invalidation that triggers deferred invalidation
    // registered with the outer analysis manager proxy for this function.
    if (auto *OuterProxy =
            InnerAM->getCachedResult<ModuleAnalysisManagerFunctionProxy>(F))
      for (const auto &OuterInvalidationPair :
           OuterProxy->getOuterInvalidations()) {
        AnalysisKey *OuterAnalysisID = OuterInvalidationPair.first;
        const auto &InnerAnalysisIDs = OuterInvalidationPair.second;
        if (Inv.invalidate(OuterAnalysisID, M, PA)) {
          if (!FunctionPA)
            FunctionPA = PA;
          for (AnalysisKey *InnerAnalysisID : InnerAnalysisIDs)
            FunctionPA->abandon(InnerAnalysisID);
        }
      }

    // Check if we needed a custom PA set, and if so we'll need to run the
    // inner invalidation.
    if (FunctionPA) {
      InnerAM->invalidate(F, *FunctionPA);
      continue;
    }

    // Otherwise we only need to do invalidation if the original PA set didn't
    // preserve all function analyses.
    if (!AreFunctionAnalysesPreserved)
      InnerAM->invalidate(F, PA);
  }

  // Return false to indicate that this result is still a valid proxy.
  return false;
}
} // namespace llvm

void ModuleToFunctionPassAdaptor::printPipeline(
    raw_ostream &OS, function_ref<StringRef(StringRef)> MapClassName2PassName) {
  OS << "function";
  if (EagerlyInvalidate)
    OS << "<eager-inv>";
  OS << '(';
  Pass->printPipeline(OS, MapClassName2PassName);
  OS << ')';
}

PreservedAnalyses ModuleToFunctionPassAdaptor::run(Module &M,
                                                   ModuleAnalysisManager &AM) {
  FunctionAnalysisManager &FAM =
      AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();

  // Request PassInstrumentation from analysis manager, will use it to run
  // instrumenting callbacks for the passes later.
  PassInstrumentation PI = AM.getResult<PassInstrumentationAnalysis>(M);

  PreservedAnalyses PA = PreservedAnalyses::all();
  for (Function &F : M) {

    if (F.isDeclaration())
      continue;
    markFragile(M);
    // Check the PassInstrumentation's BeforePass callbacks before running the
    // pass, skip its execution completely if asked to (callback returns
    // false).
    if (!PI.runBeforePass<Function>(*Pass, F))
      continue;

    PreservedAnalyses PassPA = Pass->run(F, FAM);

    // We know that the function pass couldn't have invalidated any other
    // function's analyses (that's the contract of a function pass), so
    // directly handle the function analysis manager's invalidation here.
    FAM.invalidate(F, EagerlyInvalidate ? PreservedAnalyses::none() : PassPA);

    PI.runAfterPass(*Pass, F, PassPA);

    // Then intersect the preserved set so that invalidation of module
    // analyses will eventually occur when the module pass completes.
    PA.intersect(std::move(PassPA));
  }
  displayFragile(M);

  // The FunctionAnalysisManagerModuleProxy is preserved because (we assume)
  // the function passes we ran didn't add or remove any functions.
  //
  // We also preserve all analyses on Functions, because we did all the
  // invalidation we needed to do above.
  PA.preserveSet<AllAnalysesOn<Function>>();
  PA.preserve<FunctionAnalysisManagerModuleProxy>();
  return PA;
}

AnalysisSetKey CFGAnalyses::SetKey;

AnalysisSetKey PreservedAnalyses::AllAnalysesKey;
