#pragma once

#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"

class PerlClangTidyCheck : public clang::tidy::ClangTidyCheck
{
  bool UseMultiplicity;
 public:
  PerlClangTidyCheck(llvm::StringRef Name, clang::tidy::ClangTidyContext* Context);

  [[nodiscard]] bool
    useMultiplicity() const {
    return UseMultiplicity;
  }
};
