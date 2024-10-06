#include "PerlUndefSetsvCheck.h"
#include "llvm/Support/FormatVariadic.h"
#include <sstream>
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

PerlUndefSetsvCheck::PerlUndefSetsvCheck(
    StringRef Name, ClangTidyContext* Context
    ) :
    ClangTidyCheck(Name, Context),
    UseMultiplicity(Options.getLocalOrGlobal("PerlCheckMultiplicity", 0))
{
}  

void PerlUndefSetsvCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        callExpr(
            isExpandedFromMacro("sv_setsv"),
            hasArgument(0+UseMultiplicity, expr().bind("sv")),
            hasArgument(1+UseMultiplicity,
                unaryOperator(
                    hasOperatorName("&"),
                    hasUnaryOperand(isExpandedFromMacro("PL_sv_undef"))
                )
            )
        ).bind("call"), this);
}

void PerlUndefSetsvCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    const auto *svExpr = Result.Nodes.getNodeAs<Expr>("sv");

    const LangOptions &Opts = getLangOpts();

    auto argString = [this, matchedCall, Opts, Result](auto ArgNum){
      return Lexer::getSourceText(
          CharSourceRange::getTokenRange(
              matchedCall->getArg(UseMultiplicity+ArgNum)->getSourceRange()
          ),
          *Result.SourceManager, Opts
       ).operator std::string_view();
    };
    std::string repl = llvm::formatv("sv_set_undef({0})", argString(0));
    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "sv_setsv(..., &PL_sv_undef) better written as sv_set_undef()") << hint;
}

