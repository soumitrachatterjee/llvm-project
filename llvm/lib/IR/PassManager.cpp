//===- PassManager.cpp - Infrastructure for managing & running IR passes --===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "llvm/IR/PassManager.h"
#include "llvm/Analysis/CallGraph.h" // Include the appropriate header file
#include "llvm/IR/Instructions.h"
#include "llvm/IR/PassManagerImpl.h"
#include <cstdlib>
#include <cxxabi.h>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
   #include <fcntl.h> // for open() function
#include "llvm/IR/Attributes.h"
#define limit 2
#include "llvm/ADT/StringRef.h"

#include "llvm/IR/Module.h"

#include <llvm/IR/DebugLoc.h>

// #include <llvm/IR/DebugInfoMetadata.h>
// // Opens file to store names of fragile functions.
// std::ofstream file("/ptmp/shash/llvm-project/fragile_functions.txt",
//                    std::ios::app);
// llvm::cl::OptionCategory mycat("my_category");
// llvm::cl::opt<std::string> filename{
//     "readfile",
//     llvm::cl::desc(
//         "Reads given file with line ranges for identifying fragile functions"),
//     llvm::cl::value_desc("input"), llvm::cl::cat(mycat)};
// llvm::Attribute getFragileAttr(llvm::LLVMContext &Ctx) {

//   return llvm::Attribute::get(Ctx, "isFragile", "true");
// }
// llvm::Attribute getPotentialFragileAttr(llvm::LLVMContext &Ctx) {

//   return llvm::Attribute::get(Ctx, "potentiallyFragile", "true");
// }
// struct FragileCluster {

//   int start;

//   int end;
// };

// struct FragileCluster s;

using namespace llvm;
using namespace std;
// vector<FragileCluster> vec;
// // Reads data from the file and populates a vector 'vec' with start and end
// // values.

// string demangle(const std::string &mangled_name) {
//   int status;
//   char *demangled_name =
//       abi::__cxa_demangle(mangled_name.c_str(), nullptr, nullptr, &status);
//   std::string result;
//   if (status == 0) {
//     result = demangled_name;
//     free(demangled_name);
//   } else {
//     // If demangling failed, return the original mangled name
//     result = mangled_name;
//   }
//   return result;
// }

// void getcalledfunction(Module &M, Function &F,int n)
// {
//   if(n==0)
// {
// return;
// }       
// else{

//   llvm::CallGraph CG(M);
//         for (auto &CGN : CG) {
//           llvm::Function *CallerFunction = CGN.second->getFunction();
//           if (CallerFunction) {
//             for (auto &CallRecord : *CGN.second) {
//               if (CallRecord.second->getFunction() == &F && !CallerFunction->hasFnAttribute ("potentiallyFragile")) {
//                 CallerFunction->addFnAttr(getPotentialFragileAttr(CallerFunction->getContext()));
//                 std::string tabs(limit+1 - n, '\t');
//                 file <<tabs<< demangle(CallerFunction->getName().str()) <<"\n";
//                 getcalledfunction(M,*CallerFunction,n-1);
//               }

//             }
//           }
//         }
//         }   
        
// }
// void readfile() {
//   vec.clear();
//   string line, value;
//   if (!filename.empty()) {
//     // Reads the churn data from the file specified in command-line and stores
//     // it in vector.
//     ifstream file(filename);
//     if (file.is_open()) {
//       while (getline(file, line)) {
//         stringstream obj_ss(line);
//         getline(obj_ss, value, ',');
//         s.start = stoi(value);
//         getline(obj_ss, value, ',');
//         s.end = stoi(value);
//         vec.push_back(s);
//       }
//       file.close();
//     } else {
//       cerr << "Sorry!! Unable to open file!" << endl;
//     }
//   }
// }
// // Marks functions as fragile if they overlap with line ranges in the
// // vector and prints their demangled names to a file.
//  void markFragile(Module &M) {
//   int Line = 0, Line1 = 0;
//   string sourcename = "";
//   for (Function &F : M) {
//     if (F.isDeclaration())
//       continue;

//     int flag = 0, flag1 = 0;
//     if (auto *SP = F.getSubprogram()) {
//       if (!SP->getFilename().empty()) {
//         sourcename = SP->getFilename();
//       }
//     }

//     // if (sourcename == "llvm/lib/Analysis/ScalarEvolution.cpp" &&
//     //     F.getName().str().find("__cxx_global_var_init") != 0 &&
//     //     F.getName().str() != "_GLOBAL__sub_I_ScalarEvolution.cpp") {

//   //  int fd = open("/ptmp/shash/llvm-project/callgraph.txt",
//   //                O_WRONLY | O_CREAT | O_TRUNC, 0644);

//   //  if (fd == -1) {
//   //    llvm::errs() << "Error: Unable to open the file for writing.\n";
//   //  } else {
//   //    // Open the file using llvm::raw_fd_ostream
//   //    llvm::raw_fd_ostream llvmOut(fd, /*shouldClose=*/true,
//   //                                 /*unbuffered=*/false);

//   //    // Print the call graph to the file
//   //    CG.print(llvmOut);
//   //  }

//       readfile();
//       for (BasicBlock &BB : F) {
//         for (Instruction &I : BB) {
//           if (flag == 0) {
//             if (DILocation *Loc = I.getDebugLoc()) {
//               // Stores the line number where the function starts.
//               Line = Loc->getLine();
//               flag = 1;
//               break;
//             }
//           }
//         }

//         Instruction &I1 = *(BB.getTerminator());
//         if (DILocation *Loc = I1.getDebugLoc()) {
//           if (Line != 0)
//             // Stores the line number where the function ends.
//             Line1 = Loc->getLine();
//         }
//       }
//       // Logic to find if there is an overlapping.
//       for (auto &s : vec) {
//         if (((Line >= s.start && Line <= s.end) ||
//              (Line1 >= s.start && Line1 <= s.end)) ||
//             ((s.start >= Line && s.start <= Line1) ||
//              (s.end >= Line && s.end <= Line1))) {

//           flag1 = 1;
//           break;
//         }
//       }
//       if (flag1 == 1) {
//         // Marks the function with isFragile attribute if there is an
//         // overlapping between the given line ranges and the function.
//         F.addFnAttr(getFragileAttr(F.getContext()));
//         file << demangle(F.getName().str()) <<"\n";
//         getcalledfunction(M,F,limit);
//         file<<"\n--------------------------------\n";
//       }
//       //}
//     // }
//    }
// }
// void displayFragile(Module &M) {

      

  //     if (file.is_open()) {
        // string func_name = F.getName().str();
        // file << "Function Name: ";
        // file << func_name;
        // file << "\n";

  //   file << "Signature: ";
  //   llvm::Type *returnType =
  //       F.getReturnType(); // Get the return type of the function

  //   string returnTypeStr;
  //   llvm::raw_string_ostream returnTypeStream(returnTypeStr);
  //   returnType->print(returnTypeStream); // Print the type to a stream
  //   returnTypeStream.flush(); // Flush the stream to convert to string

  //   file << returnTypeStr;
  //   file << " " << func_name;
  //   file << "(";
  // }

  // int first = 0;
  // for (llvm::Argument &Arg : F.args()) {
  //   llvm::Type *returnType =
  //       Arg.getType(); // Get the return type of the function
  //   string returnTypeStr;
  //   llvm::raw_string_ostream returnTypeStream(returnTypeStr);
  //   returnType->print(returnTypeStream); // Print the type to a stream
  //   returnTypeStream.flush(); // Flush the stream to convert to string

  //   file << returnTypeStr;
  //   string argname = Arg.getName().str();
  //   if (first == 0) {

  //     file << returnTypeStr;
  //     file << " ";

  //     file << argname;
  //     first = 1;
  //   } else {
  //     file << " ,";
  //     file << returnTypeStr;
  //     file << " ";
  //     file << argname;
  //   }
  //}
  // file << ")";
  // file << "\n";
  // int Line = 0, Line1 = 0;
  // BasicBlock &firstBB = F.front();
  // BasicBlock &lastBB = F.back();
  // for (Instruction &I : firstBB) {
  //   if (DILocation *Loc = I.getDebugLoc()) {
  //     Line = Loc->getLine();
  //     break;
  //   }
  // }
  // Instruction &I1 = *(lastBB.getTerminator());
  // if (DILocation *Loc = I1.getDebugLoc()) {
  //   Line1 = Loc->getLine();
  // }
  // file << "Size of code: ";
  // file << Line1 - Line + 1;
  // file << " Lines of code";
  // file << "\n";

  // if (DISubprogram *SP = F.getSubprogram()) {

  //   file << "Source File: ";
  //   string sf = SP->getFilename().str();
  //   file << sf;
  //   file << "\n";
  // }
  // file << "---------------------------------------------";
  // file << "\n";
  // file.close();
  //     }
  //   }
 

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
    optional<PreservedAnalyses> FunctionPA;

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
  errs()<<"print pipeline\n";

}

PreservedAnalyses ModuleToFunctionPassAdaptor::run(Module &M,
                                                   ModuleAnalysisManager &AM) {
  FunctionAnalysisManager &FAM =
      AM.getResult<FunctionAnalysisManagerModuleProxy>(M).getManager();

  // Request PassInstrumentation from analysis manager, will use it to run
  // instrumenting callbacks for the passes later.
  PassInstrumentation PI = AM.getResult<PassInstrumentationAnalysis>(M);
  // markFragile(M);
  //displayFragile(M);
  
  
  PreservedAnalyses PA = PreservedAnalyses::all();
  for (Function &F : M) {

    if (F.isDeclaration())
      continue;

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
