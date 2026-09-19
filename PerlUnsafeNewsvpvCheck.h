#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"

class PerlUnsafeNewsvpvCheck : public clang::tidy::ClangTidyCheck
{
  bool UseMultiplicity;
public:
  // Construct a new check
  //  Name, Context - passed to base constructor
  //
  PerlUnsafeNewsvpvCheck(llvm::StringRef Name,
                 clang::tidy::ClangTidyContext* Context);
  void registerMatchers(clang::ast_matchers::MatchFinder* Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult& Result) override;
};
