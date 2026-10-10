#pragma once

#include "clang-tidy/ClangTidyCheck.h"

class PerlConfig {
  bool UseMultiplicity;
 public:
  PerlConfig(clang::tidy::ClangTidyCheck::OptionsView &Options);
  [[nodiscard]] bool useMultiplicity() const {
    return UseMultiplicity;
  }
};
