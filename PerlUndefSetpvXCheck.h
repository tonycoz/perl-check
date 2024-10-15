#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include <string>
#include <vector>

class PerlUndefSetpvXCheck : public clang::tidy::ClangTidyCheck
{
  std::string PvMacro;
  bool UseMultiplicity;
public:
  // Construct a new check
  //  Name, Context - passed to base constructor
  //  PvMacro - name of macro, such as "sv_setpv"
  //
  PerlUndefSetpvXCheck(llvm::StringRef Name,
                       clang::tidy::ClangTidyContext* Context,
                       llvm::StringRef PvMacro_);
  void registerMatchers(clang::ast_matchers::MatchFinder* Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult& Result) override;
};
