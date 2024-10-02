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

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

constexpr auto use_threads = 0;

class PerlLiteralFunctionCheck : public ClangTidyCheck
{
public:
    PerlLiteralFunctionCheck(StringRef Name, ClangTidyContext* Context) : ClangTidyCheck(Name, Context)
    {
    }
    void registerMatchers(ast_matchers::MatchFinder* Finder) override;
    void check(const ast_matchers::MatchFinder::MatchResult& Result) override;
};

void PerlLiteralFunctionCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
        callExpr(isExpandedFromMacro("sv_setpvn"),
                 hasArgument(1+use_threads,
                             traverse(TK_IgnoreUnlessSpelledInSource,
                                      stringLiteral().bind("literal"))
                 ),
                 hasArgument(2+use_threads, anything())
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

    const auto sizeArg = Args[numArgs-1]->IgnoreUnlessSpelledInSource();
    Expr::EvalResult sizeIntResult;
    if (!sizeArg->EvaluateAsInt(sizeIntResult, *Result.Context)
        || !sizeIntResult.Val.isInt())
      return;
    const auto sizeInt = sizeIntResult.Val.getInt();
    if (sizeInt > strLit->getLength())
      diag(matchedCall->getExprLoc(), "length too long for literal");
    if (sizeInt != strLit->getLength())
      return;
    const auto *svArg = Args[use_threads];
    const LangOptions &Opts = getLangOpts();
    const auto svArgText = Lexer::getSourceText(CharSourceRange::getTokenRange(svArg->getSourceRange()), *Result.SourceManager, Opts);
    const auto litText = Lexer::getSourceText(CharSourceRange::getTokenRange(strLit->getSourceRange()), *Result.SourceManager, Opts);
    std::string repl = formatv("sv_setpvs({0}, {1})", svArgText, litText);
    auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "sv_setpvn() with literal better written as sv_setpvs()")
      << hint;
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
            return std::make_unique<PerlLiteralFunctionCheck>(Name, Context);
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
