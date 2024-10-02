#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
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
                                      stringLiteral())
                 )
        ).bind("perl_check_literal_functions"), this);
}

void PerlLiteralFunctionCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto* MatchedCall = Result.Nodes.getNodeAs<CallExpr>("perl_check_literal_functions");
    // this is the expanded arguments which may include aTHX
    auto numArgs = MatchedCall->getNumArgs();
    if (numArgs < 2)
      return; // something is strange

    const auto *Args = MatchedCall->getArgs();
    const auto strArg = Args[numArgs-2]->IgnoreUnlessSpelledInSource();
    assert(strArg->getStmtClass() == Stmt::StringLiteralClass);

    const auto strLit = static_cast<const StringLiteral *>(strArg);

    const auto sizeArg = Args[numArgs-1]->IgnoreUnlessSpelledInSource();
    Expr::EvalResult sizeIntResult;
    if (!sizeArg->EvaluateAsInt(sizeIntResult, *Result.Context)
        || !sizeIntResult.Val.isInt())
      return;
    const auto sizeInt = sizeIntResult.Val.getInt();
    if (sizeInt > strLit->getLength())
      diag(MatchedCall->getExprLoc(), "length too long for literal");
    if (sizeInt != strLit->getLength())
      return;
    diag(MatchedCall->getExprLoc(), "sv_setpvn() with literal better written as sv_setpvs()");
}

namespace {

class PerlCheckModule : public ClangTidyModule
{
public:
    void addCheckFactories(ClangTidyCheckFactories& CheckFactories) override
    {
        CheckFactories.registerCheck<PerlLiteralFunctionCheck>("perl-literal-sv_setpvn");
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
