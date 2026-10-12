#include "PerlCheck.h"
#include "PerlFreeSvCheck.h"
#include "llvm/Support/FormatVariadic.h"
#include <cassert>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;
using namespace perl_check;

PerlFreeSvCheck::PerlFreeSvCheck(
    StringRef Name, ClangTidyContext* Context
    ) :
    ClangTidyCheck(Name, Context)
{
}  

void PerlFreeSvCheck::registerMatchers(MatchFinder* Finder)
{
    Finder->addMatcher(
      callExpr(
        isExpandedFromMacro("Safefree"),
         hasArgument(
          0,
          ignoringParenCasts(
            expr(hasType(
              pointerType(pointee(
                typedefType(hasDeclaration(
                  typedefNameDecl(hasAnyName("SV", "AV", "HV", "CV", "IO", "GV"))
                ))
              ))
            ))
        )
      )
    ).bind("call"), this);
}

void PerlFreeSvCheck::check(const MatchFinder::MatchResult& Result)
{
    const auto *matchedCall = Result.Nodes.getNodeAs<CallExpr>("call");
    //const auto *svExpr = Result.Nodes.getNodeAs<Expr>("sv");

    const LangOptions &Opts = getLangOpts();

    //auto argString = getArgText(matchedCall, Result, Opts, 0);
    //std::string repl = llvm::formatv("SvREFCNT_dec({0})", argString(0));
    //auto hint = FixItHint::CreateReplacement(matchedCall->getSourceRange(), repl);
    diag(matchedCall->getExprLoc(), "Safefree() of an SV * should use SvREFCNT_dec()");// << hint;
}

