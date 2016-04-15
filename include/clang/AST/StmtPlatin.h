#ifndef LLVM_CLANG_AST_STMTPLATIN_H
#define LLVM_CLANG_AST_STMTPLATIN_H

#include "clang/Basic/SourceLocation.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Stmt.h"

namespace clang {

class Flowfact : public Stmt {
  friend class ASTStmtReader;

  /// \brief Location of the flowfact.
  SourceRange Range;

  Flowfact(SourceRange Range, ArrayRef<int> Multipliers,
           ArrayRef<std::string> Markers, int rhs);

public:
  static Flowfact *Create(const ASTContext &C, SourceRange Range,
      ArrayRef<int> Multipliers, ArrayRef<std::string> Markers, int rhs);
  static Flowfact *CreateEmpty(const ASTContext &C);

  /// \brief Returns starting location of the flowfact.
  SourceLocation getLocStart() const { return Range.getBegin(); }
  /// \brief Returns ending location of the flowfact.
  SourceLocation getLocEnd() const { return Range.getEnd(); }

  child_range children() {
    return child_range(child_iterator(), child_iterator());
  }

  unsigned getNumLhsTerms() const { return Multipliers.size(); }

  static bool classof(const Stmt *T) {
    return T->getStmtClass() == FlowfactClass;
  }

  std::vector<int> Multipliers;
  std::vector<std::string> Markers;
  int RHS;
};

}  // end namespace clang

#endif
