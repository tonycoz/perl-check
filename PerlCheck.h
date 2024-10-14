#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include <string_view>

namespace {
  using namespace clang;
  using namespace clang::ast_matchers;
  auto
    getArgText(const CallExpr *call, const MatchFinder::MatchResult &Result,
               const LangOptions &Opts, const bool UseMultiplicity) {
    auto *SourceManager = Result.SourceManager;
    return [=, &Result, &Opts](int ArgNum){
      return Lexer::getSourceText(
          CharSourceRange::getTokenRange(
              call->getArg(UseMultiplicity+ArgNum)->getSourceRange()
          ),
          *SourceManager, Opts
       ).operator std::string_view();
    };
  }
}
