#include "PerlClangTidyCheck.h"
#include "PerlConfig.h"

namespace {
  const PerlConfig &
  getConfig(clang::tidy::ClangTidyCheck::OptionsView &Options) {
    static PerlConfig config{Options};
    return config;
  }
}

PerlClangTidyCheck::PerlClangTidyCheck(
  llvm::StringRef Name,
  clang::tidy::ClangTidyContext* Context) :
  ClangTidyCheck(Name, Context),
  UseMultiplicity(getConfig(Options).useMultiplicity())
{
}
