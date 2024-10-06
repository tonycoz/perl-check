#include "PerlLiteralFunctionCheck.h"
#include <sstream>
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

PerlLiteralFunctionCheck::PerlLiteralFunctionCheck(
    StringRef Name, ClangTidyContext* Context,
    StringRef PvnMacro_, StringRef PvsMacro_,
    int LiteralArgNum_, int LengthArgNum_,
    std::vector<int> &&KeepArgs_) :
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
                 hasArgument(LiteralArgNum+UseMultiplicity,
                             traverse(TK_IgnoreUnlessSpelledInSource,
                                      stringLiteral().bind("literal"))
                 ),
                 hasArgument(LengthArgNum+UseMultiplicity, anything())
        ).bind("call"), this);
}

void PerlLiteralFunctionCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    const auto *strLit = Result.Nodes.getNodeAs<StringLiteral>("literal");

    const auto sizeArg = matchedCall->getArg(UseMultiplicity+LengthArgNum)
      ->IgnoreUnlessSpelledInSource();
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

    auto argString = [this, matchedCall, Opts, Result](auto ArgNum){
      return Lexer::getSourceText(
          CharSourceRange::getTokenRange(
              matchedCall->getArg(UseMultiplicity+ArgNum)->getSourceRange()
          ),
          *Result.SourceManager, Opts
       ).operator std::string_view();
    };
    // I'm not entired happy with this, iostreams has significant
    // overhead, but using accumulate() here would (I think) be
    // theoretically quadratic time as the expanding string would be
    // reallocated and expanded at each step.
    //
    // Of course the N is always small here so it practically wouldn't
    // matter.

    std::ostringstream srepl;
    srepl << PvsMacro << '(' << argString(KeepArgs[0]);
    for (auto i = std::next(KeepArgs.begin()); i != KeepArgs.end(); ++i) {
      srepl << ", " << argString(*i);
    }
    srepl << ')';
    std::string repl = srepl.str();
    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "%0() with literal better written as %1()")
      << PvnMacro << PvsMacro << hint;
}

