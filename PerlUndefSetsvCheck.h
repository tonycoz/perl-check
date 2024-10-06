#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include <string>
#include <vector>

class PerlUndefSetsvCheck : public clang::tidy::ClangTidyCheck
{
  // Initialized from PerlCheckMulitplicity in the configuration.
  // This should reflect whether -Dusemultiplicity is enabled for the
  // build, note that -Dusethreads also turns on multiplicity
  //
  // I looked at initializing this from the macro definitions but
  // we see that too late if we want to use the AST matchers for more
  // than matching the macro name.
  bool UseMultiplicity;
public:
  // Construct a new check
  //  Name, Context - passed to base constructor
  //
  PerlUndefSetsvCheck(llvm::StringRef Name,
                 clang::tidy::ClangTidyContext* Context);
  void registerMatchers(clang::ast_matchers::MatchFinder* Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult& Result) override;
};
