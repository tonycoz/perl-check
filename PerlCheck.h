#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include <string_view>

namespace perl_check {
  using namespace clang;
  using namespace clang::ast_matchers;

  // a wrapper used to simplify fetching function call arguments
  class GetArgs {
    const CallExpr *m_Call;
    const MatchFinder::MatchResult *m_Result;
    const LangOptions *m_Opts;
    bool m_UseMultiplicity;
  public:
    GetArgs(const CallExpr *Call, const MatchFinder::MatchResult *pResult,
            const LangOptions *pOpts, bool UseMultiplicity)
      :m_Call(Call), m_Result(pResult), m_Opts(pOpts),
      m_UseMultiplicity(UseMultiplicity) {
    }
    llvm::StringRef operator()(int index) const {
      auto *SourceManager = m_Result->SourceManager;
      return Lexer::getSourceText(
        CharSourceRange::getTokenRange(
          m_Call->getArg(m_UseMultiplicity+index)->getSourceRange()
        ),
        *SourceManager, *m_Opts
       );
    }
  };
  inline GetArgs getArgText(const CallExpr *call,
                     const MatchFinder::MatchResult &Result,
                     const LangOptions &Opts, bool UseMultiplicity) {
    return GetArgs{call, &Result, &Opts, UseMultiplicity};
  }

  extern std::string
    MakeCall(llvm::StringRef NewName, const GetArgs &Args,
             const llvm::SmallVector<int> &KeepArgs,
             int InFlagsArgNum,
             llvm::StringRef DefaultFlags, bool RealArgs);
}
