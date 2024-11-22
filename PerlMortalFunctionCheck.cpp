#include "PerlCheck.h"
#include "PerlMortalFunctionCheck.h"
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

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

  // srepl << Lexer::getSourceText(
  //         CharSourceRange::getTokenRange(
  //             matchedCall->getSourceRange()
  //         ),
  //         *Result.SourceManager, Opts
  //                              );
  auto addHint = [&](auto &d){
    if (DoFixIt) {
      // I want a better way to do this
      // clang::DiagnosticBuilder (returned by diag) is nethier copiable
      // not movable, so I can't use a common variable for the two
      // branches of the if DefaultFlags.size
      auto argString = getArgText(matchedCall, Result, Opts, UseMultiplicity);

      std::string repl;
      llvm::raw_string_ostream srepl{repl};
      srepl.reserveExtraSpace(80); // typically enough
  
      assert(KeepArgs[0] != -1); // don't handle flags in first parameter
      srepl << ReplacementMacro << '(' << argString(KeepArgs[0]);
      for (auto i = std::next(KeepArgs.begin()); i != KeepArgs.end(); ++i) {
        srepl << ", ";
        if (*i == -1) {
          if (InFlagsArgNum == -1) {
            srepl << DefaultFlags;
          }
          else {
            srepl << argString(*i) << " | SVs_TEMP";
          }
        }
        else {
          srepl << argString(*i);
        }
      }
      srepl << ')';
      auto hint = FixItHint::CreateReplacement(sv_2mortalCall->getSourceRange(), repl);
      d << hint;
    }
  };
  if (DefaultFlags.size()) {
    addHint(diag(sv_2mortalCall->getExprLoc(), "sv_2mortal(%0(...)) better written with SVs_TEMP")
            << OrigMacro);
  }
  else {
    addHint(diag(sv_2mortalCall->getExprLoc(), "sv_2mortal(%0(...)) better written as %1()")
            << OrigMacro << ReplacementMacro);
  }
}
