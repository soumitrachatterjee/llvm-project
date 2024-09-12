// Usage: ./clang -cc1 -load our_fdump_auto_inference.so -plugin -fdump-auto-type-inference example.cpp -plugin-arg-hello arguments


#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/Lex/Pragma.h"
#include "clang/Lex/Preprocessor.h"


class HelloVisitor : public clang::RecursiveASTVisitor<HelloVisitor> {
public:
  bool VisitDecl(clang::Decl *D) {
    if(const clang::VarDecl *varDecl = llvm::dyn_cast<clang::VarDecl>(D)){
    	if (varDecl->getType()->getTypeClass() == clang::Type::Auto) {
            const clang::AutoType *autoType = varDecl->getType()->getAs<clang::AutoType>();
            const clang::QualType deducedType = autoType->getDeducedType();
            llvm::errs() << "note: type of '" << varDecl->getNameAsString() << "' deduced as '" << deducedType.getAsString() << "'" <<"\n";
        }
    }
    return true;
  }
};

class HelloConsumer : public clang::ASTConsumer {
public:
  void HandleTranslationUnit(clang::ASTContext &Context) override {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

private:
  HelloVisitor Visitor;
};

class HelloAction : public clang::PluginASTAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
      return std::unique_ptr<clang::ASTConsumer>(new HelloConsumer);
    }

protected:
  bool ParseArgs(const clang::CompilerInstance &Compiler, 
                 const std::vector<std::string> &args) override {
    return true;
  }
};

static clang::FrontendPluginRegistry::Add<HelloAction>
X("-fdump-auto-type-inference", "Dump the C++ auto inferences");
