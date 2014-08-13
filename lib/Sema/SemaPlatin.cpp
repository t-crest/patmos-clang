#include "clang/AST/StmtPlatin.h"
#include "clang/Parse/Parser.h"

using namespace clang;

StmtResult Sema::ActOnFlowfact(SourceLocation StartLoc,
                               SourceLocation EndLoc,
                               ArrayRef<int> Multipliers,
                               ArrayRef<std::string> Markers,
                               int rhs) {
  return Flowfact::Create(Context, SourceRange(StartLoc, EndLoc),
                          Multipliers, Markers, rhs);
}

