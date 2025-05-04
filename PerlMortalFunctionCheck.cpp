#include "PerlCheck.h"
#include "PerlMortalFunctionCheck.h"
#include <cassert>
#include <iostream>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;
using namespace perl_check;


PerlMortalFunctionCheck::PerlMortalFunctionCheck(
    StringRef Name, ClangTidyContext* Context,
    StringRef OrigMacro_, StringRef ReplacementMacro_,
    int InFlagsArgNum_,
    llvm::SmallVector<int> &&KeepArgs_,
    std::string_view DefaultFlags_,
    bool DoFixIt_) :
    ClangTidyCheck(Name, Context),
    UseMultiplicity(Options.getLocalOrGlobal("PerlCheckMultiplicity", 0)),
    OrigMacro(OrigMacro_),
    ReplacementMacro(ReplacementMacro_),
    InFlagsArgNum(InFlagsArgNum_),
    KeepArgs(KeepArgs_),
    DefaultFlags(DefaultFlags_),
    DoFixIt(DoFixIt_)
{
  // I used to assert on flags being on input or in DefaultFlags, but
  // the sv_mortalcopy() replacement doesn't need this.
  assert(!KeepArgs.empty());
}  

void PerlMortalFunctionCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        // Perl_sv_2mortal(aTHX_ OrigMacro(...))
        callExpr(
            isExpandedFromMacro("sv_2mortal"),
            hasArgument(
                UseMultiplicity,
                traverse(
                    TK_IgnoreUnlessSpelledInSource,
                    callExpr(
                        isExpandedFromMacro(OrigMacro)
                    ).bind("call")
                )
            )
        ).bind("sv2mortal"), this);
}

void PerlMortalFunctionCheck::check(const MatchFinder::MatchResult& Result)
{
  const auto * const sv_2mortalCall =
    Result.Nodes.getNodeAs<CallExpr>("sv2mortal");
  const auto * const matchedCall =
    Result.Nodes.getNodeAs<CallExpr>("call");
  const LangOptions &Opts = getLangOpts();

  auto argString = getArgText(matchedCall, Result, Opts, UseMultiplicity);
  std::string repl = MakeCall(ReplacementMacro, argString, KeepArgs,
                              InFlagsArgNum, DefaultFlags, DoFixIt);

  if (DoFixIt) {
    diag(sv_2mortalCall->getExprLoc(), "sv_2mortal(%0(...)) better written as %1() with %2")
      << OrigMacro << ReplacementMacro << DefaultFlags
      << FixItHint::CreateReplacement(sv_2mortalCall->getSourceRange(), repl);
  }
  else if (DefaultFlags.size()) {
    diag(sv_2mortalCall->getExprLoc(), "sv_2mortal(%0(...)) better written as %1() with %2") << OrigMacro << ReplacementMacro << DefaultFlags;
  }
  else {
    diag(sv_2mortalCall->getExprLoc(), "sv_2mortal(%0(...) better written as %1()")
      << OrigMacro << ReplacementMacro;
  }
}
