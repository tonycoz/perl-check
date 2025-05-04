#include "PerlCheck.h"
#include "PerlLiteralFunctionCheck.h"
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;
using namespace perl_check;

PerlLiteralFunctionCheck::PerlLiteralFunctionCheck(
    StringRef Name, ClangTidyContext* Context,
    StringRef PvnMacro_, StringRef PvsMacro_,
    int LiteralArgNum_, int LengthArgNum_,
    llvm::SmallVector<int> &&KeepArgs_) :
    ClangTidyCheck(Name, Context),
    UseMultiplicity(Options.getLocalOrGlobal("PerlCheckMultiplicity", 0)),
    PvnMacro(PvnMacro_),
    PvsMacro(PvsMacro_),
    LiteralArgNum(LiteralArgNum_),
    LengthArgNum(LengthArgNum_),
    KeepArgs(KeepArgs_)
{
  assert(KeepArgs.size() > 0);
}  

void PerlLiteralFunctionCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        callExpr(isExpandedFromMacro(PvnMacro),
                 unless(isExpandedFromMacro(PvsMacro)),
                 hasArgument(LiteralArgNum+UseMultiplicity,
                             traverse(TK_IgnoreUnlessSpelledInSource,
                                      stringLiteral().bind("literal"))
                 ),
                 hasArgument(LengthArgNum+UseMultiplicity,
                             expr().bind("size"))
        ).bind("call"), this);
}

void PerlLiteralFunctionCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    const auto *strLit = Result.Nodes.getNodeAs<StringLiteral>("literal");

    const auto sizeArg = Result.Nodes.getNodeAs<Expr>("size");
    Expr::EvalResult sizeIntResult;
    if (!sizeArg->EvaluateAsInt(sizeIntResult, *Result.Context)
        || !sizeIntResult.Val.isInt())
      return;
    const auto sizeInt = sizeIntResult.Val.getInt();
    if (sizeInt > strLit->getLength())
      diag(matchedCall->getExprLoc(), "length too long for literal");
    if (sizeInt != strLit->getLength())
      return;
    const LangOptions &Opts = getLangOpts();

    auto argString = getArgText(matchedCall, Result, Opts, UseMultiplicity);

    std::string repl;
    llvm::raw_string_ostream srepl{repl};
    srepl.reserveExtraSpace(80); // typically enough

    srepl << PvsMacro << '(' << argString(KeepArgs[0]);
    for (auto i = std::next(KeepArgs.begin()); i != KeepArgs.end(); ++i) {
      srepl << ", " << argString(*i);
    }
    srepl << ')';

    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "%0() with literal better written as %1()")
      << PvnMacro << PvsMacro << hint;
}

