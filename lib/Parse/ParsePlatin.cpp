
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/StmtPlatin.h"
#include "clang/Parse/ParseDiagnostic.h"
#include "clang/Parse/Parser.h"
#include "llvm/Support/Debug.h"
#include "RAIIObjectsForParser.h"

using namespace clang;

StmtResult Parser::ParsePlatinPragma() {
  assert(Tok.is(tok::annot_pragma_platinff) && "Not a platin flowfact");
  SourceLocation Loc = ConsumeToken();
  SmallVector<std::string, 2> Markers;
  SmallVector<int, 2> Multipliers;

  // Parse '('.
  BalancedDelimiterTracker T(*this, tok::l_paren,
                             tok::annot_pragma_platinff_end);
  if (T.expectAndConsume(diag::err_expected_lparen_after,
                         "platin"))
    return StmtError();

  // LHS
  while (Tok.isNot(tok::lessequal) &&
         Tok.isNot(tok::annot_pragma_platinff_end)) {

    Token signTok;
    if (Tok.is(tok::plus) || Tok.is(tok::minus)) {
      signTok = Tok;
      ConsumeToken();
    }
    if (Tok.getKind() == tok::numeric_constant) {
      ExprResult Multiplier = Actions.ActOnNumericConstant(Tok);
      if (Multiplier.isInvalid())
        return StmtError();
      int m = cast<IntegerLiteral>(Multiplier.get())->getValue().getSExtValue();
      m = signTok.is(tok::minus) ? -m : m;
      Multipliers.push_back(m);
      ConsumeToken();
    } else {
      Multipliers.push_back(signTok.is(tok::minus) ? -1 : 1);
    }

    // eat the '@'-token that can lead a marker name
    std::string str;
    if (Tok.getLength() == 1 && PP.getSpelling(Tok) == "@") {
      str = "@";
      ConsumeToken();
    }

    str.append(PP.getSpelling(Tok));
    Markers.push_back(str);
    ConsumeToken();
  }

  // binary relation (currently only: <=)
  if (Tok.getKind() != tok::lessequal) {
    Diag(Tok, diag::err_pragma_platin_malformed);
    StmtError();
  }
  ConsumeToken();

  // RHS
  if (Tok.getKind() != tok::numeric_constant) {
    Diag(Tok, diag::err_pragma_platin_malformed);
    StmtError();
  }

  ExprResult RHS = Actions.ActOnNumericConstant(Tok);
  if (RHS.isInvalid())
    return StmtError();
  int RHSi = cast<IntegerLiteral>(RHS.get())->getValue().getSExtValue();
  ConsumeToken();

  // Parse ')'.
  T.consumeClose();

  assert(Tok.is(tok::annot_pragma_platinff_end) && "Missing end token");
  ConsumeToken();
  return Actions.ActOnFlowfact(Loc, Tok.getLocation(),
                               Multipliers, Markers, RHSi);
}
