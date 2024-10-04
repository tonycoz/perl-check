#include "llvm/Support/FormatVariadic.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include <string>
#include <cassert>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

class PerlLiteralFunctionCheck : public ClangTidyCheck
{
  bool UseMultiplicity;
  std::string PvnMacro;
  std::string PvsMacro;
  int LiteralArgNum;
  int LengthArgNum;
  std::vector<int> KeepArgs;
public:
  PerlLiteralFunctionCheck(StringRef Name, ClangTidyContext* Context,
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
    }
    void registerMatchers(ast_matchers::MatchFinder* Finder) override;
    void check(const ast_matchers::MatchFinder::MatchResult& Result) override;
};

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

    // this is the expanded arguments which may include aTHX
    auto numArgs = matchedCall->getNumArgs();
    if (numArgs < 2)
      return; // something is strange

    const auto *Args = matchedCall->getArgs();

    const auto sizeArg = Args[UseMultiplicity+LengthArgNum]->IgnoreUnlessSpelledInSource();
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
    std::stringstream srepl;
    auto argString = [this, Args, Opts, Result](auto ArgNum){
      return Lexer::getSourceText(
          CharSourceRange::getTokenRange(
              Args[UseMultiplicity+ArgNum]->getSourceRange()
          ),
          *Result.SourceManager, Opts
       ).operator std::string_view();
    };
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

namespace {

class PerlCheckModule : public ClangTidyModule
{
public:
    void addCheckFactories(ClangTidyCheckFactories& CheckFactories) override
    {
      CheckFactories.registerCheckFactory(
          "perl-literal-sv_setpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(Name, Context, "sv_setpvn", "sv_setpvs", 1, 2, std::vector{ 0, 1 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-newSVpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(Name, Context, "newSVpvn", "newSVpvs", 0, 1, std::vector{ 0 });
          });
    }
};

}  // namespace

namespace clang::tidy {

// Register the module using this statically initialized variable.
static ClangTidyModuleRegistry::Add<::PerlCheckModule> perlCheckInit("perl-literal-sv_setpvn",
                                                                                       "Adds 'perl-literal-sv_setpvn' check.");

// This anchor is used to force the linker to link in the generated object file and thus register the module.
volatile int perlCheckAnchorSource = 0;

}  // namespace clang::tidy
