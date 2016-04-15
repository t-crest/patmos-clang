
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtPlatin.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/ASTConsumers.h"
#include "llvm/PML.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/YAMLParser.h"
#include "llvm/Support/YAMLTraits.h"

using namespace clang;

namespace  {
  class FlowfactExporter : public ASTConsumer,
                           public RecursiveASTVisitor<FlowfactExporter> {
    ASTContext *Context;
    StringRef   OutFileName;
    llvm::yaml::PMLDoc *YDoc;
    llvm::yaml::FlowFact *FF;

  public:
    FlowfactExporter(StringRef filename)
      : OutFileName(filename), YDoc(NULL), FF(NULL) {
      if (filename.empty())
        llvm::report_fatal_error("[clang-ff] Filename for export missing");
    }
    void Initialize(ASTContext &Context) {
      using namespace llvm;
      this->Context = &Context;
      YDoc = new yaml::PMLDoc(Context.getTargetInfo().getTriple().getTriple());
      FF = new yaml::FlowFact(llvm::yaml::level_bitcode);
    }

    ~FlowfactExporter() {
      writeToFile(YDoc);
      delete FF;
      delete YDoc;
    }
    bool VisitFlowfact(Flowfact *Stmt) {
      llvm::dbgs() << "PML Export: ";
      Stmt->printPretty(llvm::dbgs(), 0, PrintingPolicy(Context->getLangOpts()));;
      exportFlowFact(Stmt);
      return true;
    }

    bool VisitFunctionDecl(FunctionDecl *f) {
      using namespace llvm;
      // Only function definitions (with bodies), not declarations.
      if (f->hasBody()) {
        // Function name
        DeclarationName DeclName = f->getNameInfo().getName();
        std::string FuncName = DeclName.getAsString();

        FF->setLoopScope(yaml::Name(FuncName), yaml::Name());
      }
      return true;
    }

    void HandleTranslationUnit(clang::ASTContext &Context) {
      TraverseDecl(Context.getTranslationUnitDecl());
    }


    void exportFlowFact(Flowfact *Stmt) {
      using namespace llvm;

      for (unsigned i = 0; i < Stmt->getNumLhsTerms(); ++i) {
        std::string name = Stmt->Markers[i];
        assert(name.length() > 0);
        if (name[0] == '@') // skip '@' in marker name
          name = name.substr(1);
        yaml::ProgramPoint *Marker =
          yaml::ProgramPoint::CreateMarker(yaml::Name(name));
        FF->addTermLHS(Marker, Stmt->Multipliers[i]);
      }
      FF->RHS = yaml::Name(Stmt->RHS);
      FF->Comparison = yaml::cmp_less_equal;
      FF->Origin = "user.bc";
      YDoc->addFlowFact(FF);
      // next flowfact
      FF = new llvm::yaml::FlowFact(llvm::yaml::level_bitcode);
    }

    void writeToFile(llvm::yaml::PMLDoc *thedoc) {
      using namespace llvm;

      tool_output_file *OutFile;
      yaml::Output *Output;
      std::error_code ErrorInfo;

      OutFile = new tool_output_file(OutFileName, ErrorInfo, sys::fs::F_Text);
      if (ErrorInfo) {
        delete OutFile;
        errs() << "[clang-ff] Opening Export File failed: " << OutFileName << "\n";
        errs() << "[clang-ff] Reason: " << ErrorInfo.message();
        return;
      }
      else {
        Output = new yaml::Output(OutFile->os());
      }
      *Output << thedoc;

      if (OutFile) {
        OutFile->keep();
        delete Output;
        delete OutFile;
      }
    }
  };
}

namespace clang {
std::unique_ptr<ASTConsumer> CreateFlowfactExporter(StringRef filename) {
  return llvm::make_unique<FlowfactExporter>(filename);
}
}
