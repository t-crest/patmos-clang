//===--- Loopbound.h - Types for Loopbound ----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_SEMA_LOOPBOUND_H
#define LLVM_CLANG_SEMA_LOOPBOUND_H

#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Sema/AttributeList.h"
#include "clang/Sema/Ownership.h"

namespace clang {

/// \brief Loopbound pragmas.
struct Loopbound {
  // Source range of the directive.
  SourceRange Range;
  // Identifier corresponding to the name of the pragma ("loopbound")
  IdentifierLoc *PragmaNameLoc;
  // Expression for the min argument
  Expr *MinExpr;
  // Expression for the max argument
  Expr *MaxExpr;

  Loopbound()
      : PragmaNameLoc(NULL), MinExpr(NULL), MaxExpr(NULL) {}
};

} // end namespace clang

#endif // LLVM_CLANG_SEMA_LOOPBOUND_H
