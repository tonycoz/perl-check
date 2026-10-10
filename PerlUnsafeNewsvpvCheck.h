#pragma once

#include "llvm/ADT/StringRef.h"
#include "PerlClangTidyCheck.h"

class PerlUnsafeNewsvpvCheck : public PerlClangTidyCheck
{
public:
  // Construct a new check
  //  Name, Context - passed to base constructor
  //
  PerlUnsafeNewsvpvCheck(llvm::StringRef Name,
                 clang::tidy::ClangTidyContext* Context);
  void registerMatchers(clang::ast_matchers::MatchFinder* Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult& Result) override;
};
