#include "PerlCheck.h"
#include "PerlClearSetpvsCheck.h"
#include "llvm/Support/FormatVariadic.h"
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;
using namespace perl_check;

PerlClearSetpvsCheck::PerlClearSetpvsCheck(
    StringRef Name, ClangTidyContext* Context
    ) :
    PerlClangTidyCheck(Name, Context)
{
}  

void PerlClearSetpvsCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        callExpr(
            isExpandedFromMacro("sv_setpvs"),
            hasArgument(0+useMultiplicity(), expr().bind("sv")),
            hasArgument(1+useMultiplicity(),
                        traverse(TK_IgnoreUnlessSpelledInSource,
                                 stringLiteral().bind("literal"))
                        ),
            hasArgument(2+useMultiplicity(),
                        expr().bind("size"))
        ).bind("call"), this);
}

void PerlClearSetpvsCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    const auto *svExpr = Result.Nodes.getNodeAs<Expr>("sv");

    const LangOptions &Opts = getLangOpts();

    const auto *strLit = Result.Nodes.getNodeAs<StringLiteral>("literal");

    const auto sizeArg = Result.Nodes.getNodeAs<Expr>("size");
    Expr::EvalResult sizeIntResult;
    if (!sizeArg->EvaluateAsInt(sizeIntResult, *Result.Context)
        || !sizeIntResult.Val.isInt())
      return;
    const auto sizeInt = sizeIntResult.Val.getInt();
    if (sizeInt != strLit->getLength() || sizeInt != 0)
      return;

    auto argString = getArgText(matchedCall, Result, Opts, useMultiplicity());
    std::string repl = llvm::formatv("SvPVCLEAR({0})", argString(0));
    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "sv_setpvs(sv, \"\") better written as SvPVCLEAR(sv)") << hint;
}

