#include "PerlCheck.h"
#include "PerlUnsafeNewsvpvCheck.h"
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;
using namespace perl_check;

PerlUnsafeNewsvpvCheck::PerlUnsafeNewsvpvCheck(
    StringRef Name, ClangTidyContext* Context) :
    PerlClangTidyCheck(Name, Context)
{
}  

void PerlUnsafeNewsvpvCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        callExpr(isExpandedFromMacro("newSVpv"),
                 hasArgument(0+useMultiplicity(), expr().bind("string")),
                 hasArgument(1+useMultiplicity(),
                             traverse(TK_IgnoreUnlessSpelledInSource,
                                      unless(integerLiteral().bind("len")))
                 )
        ).bind("call"), this);
}

void PerlUnsafeNewsvpvCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    const auto *strExpr = Result.Nodes.getNodeAs<Expr>("string");
    const auto *lenLit = Result.Nodes.getNodeAs<StringLiteral>("literal");

    const auto sizeArg = Result.Nodes.getNodeAs<Expr>("size");
    const LangOptions &Opts = getLangOpts();

    auto argString = getArgText(matchedCall, Result, Opts, useMultiplicity());

    std::string repl;
    llvm::raw_string_ostream srepl{repl};
    srepl.reserveExtraSpace(80); // typically enough

    srepl << "newSVpvn(" << argString(0) << ", " << argString(1) << ")";

    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "newSVpv() with non-literal length better written with newSVpvn()")
      << hint;
}

