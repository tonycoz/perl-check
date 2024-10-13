#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include <string>
#include <vector>
#include <string_view>

using namespace std::literals;

class PerlMortalFunctionCheck : public clang::tidy::ClangTidyCheck
{
  // Initialized from PerlCheckMultiplicity in the configuration.
  // This should reflect whether -Dusemultiplicity is enabled for the
  // build, note that -Dusethreads also turns on multiplicity
  bool UseMultiplicity;
  // name of the macro that we (may) want to change
  std::string OrigMacro;
  // name of the macro to change to
  std::string ReplacementMacro;
  // original argument index of the flags, -1 if there's no flags
  int InFlagsArgNum;
  // parameter indexes from the Perl_ function call to include in the
  // FixItHint.  Should include a -1 entry for the flags.
  std::vector<int> KeepArgs;
  // Default flags if the InFlagsArgNum is -1
  std::string_view DefaultFlags;
public:
  // Construct a new check
  //  Name, Context - passed to base constructor
  //  PvnMacro_ - macro to look for, such as sv_setpvn
  //  ReplacmentMacro_ - replacement literal string macro like sv_setpvn_flags
  //  InFlagsArgNum_ - input argument in the underlying
  //    call that contains the flags, -1 if there are no flags
  //  KeepArgs_ - int vector of arguments from the call to the Perl_
  //    prefix function end up in the FixItHint replacement
  //
  PerlMortalFunctionCheck(
      llvm::StringRef Name, clang::tidy::ClangTidyContext* Context,
      llvm::StringRef OrigMacro_, llvm::StringRef ReplacementMacro_,
      int InFlagsArgNum_,
      std::vector<int> &&KeepArgs_, std::string_view DefaultFlags = ""sv);
  void registerMatchers(clang::ast_matchers::MatchFinder* Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult& Result) override;
};
