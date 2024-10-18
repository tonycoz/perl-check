#include "PerlCheck.h"
#include "PerlUndefSetpvXCheck.h"
#include "llvm/Support/FormatVariadic.h"
#include <cassert>
#include <iostream>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

PerlUndefSetpvXCheck::PerlUndefSetpvXCheck(
    StringRef Name, ClangTidyContext* Context,
    StringRef PvMacro_,
    StringRef PvCallTo_
    ) :
    ClangTidyCheck(Name, Context),
    PvMacro(PvMacro_),
    PvCallTo(PvCallTo_),
    UseMultiplicity(Options.getLocalOrGlobal("PerlCheckMultiplicity", 0))
{
}  

void PerlUndefSetpvXCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        callExpr(
            callee(functionDecl(hasName(PvCallTo))),
            isExpandedFromMacro(PvMacro),
            hasArgument(0+UseMultiplicity, expr().bind("sv")),
            hasArgument(1+UseMultiplicity, expr().bind("pv"))
        ).bind("call"), this);
}

void PerlUndefSetpvXCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    const auto *svExpr = Result.Nodes.getNodeAs<Expr>("sv");
    const auto *pvExpr = Result.Nodes.getNodeAs<Expr>("pv");

    const LangOptions &Opts = getLangOpts();
    auto argString = getArgText(matchedCall, Result, Opts, UseMultiplicity);

    if (!pvExpr->isConstantInitializer(*Result.Context, false) ||
        !pvExpr->isNullPointerConstant(
            *Result.Context, Expr::NPC_NeverValueDependent)) {
      return;
    }

    std::string repl = llvm::formatv("sv_set_undef({0})", argString(0));
    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "%0(..., NULL) better written as sv_set_undef()") << PvMacro << hint;
}

