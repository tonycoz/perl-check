#pragma once

#include "llvm/ADT/StringRef.h"
#include "clang/Lex/Lexer.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include <string>
#include <vector>

class PerlLiteralFunctionCheck : public clang::tidy::ClangTidyCheck
{
  // Initialized from PerlCheckMulitplicity in the configuration.
  // This should reflect whether -Dusemultiplicity is enabled for the
  // build, note that -Dusethreads also turns on multiplicity
  //
  // I looked at initializing this from the macro definitions but
  // we see that too late if we want to use the AST matchers for more
  // than matching the macro name.
  bool UseMultiplicity;
  // name of the macro that we (may) want to change
  std::string PvnMacro;
  // name of the macro to change to
  std::string PvsMacro;
  // argument index of the string in the underlying Perl_ function call
  int LiteralArgNum;
  // argument index of the length of the string in the underlying Perl_
  // function call.
  int LengthArgNum;
  // parameter indexes from the Perl_ function call to include in the
  // FixItHint.
  std::vector<int> KeepArgs;
public:
  // Construct a new check
  //  Name, Context - passed to base constructor
  //  PvnMacro_ - macro to look for, such as sv_setpvn
  //  PvsMacro_ - replacement literal string macro like sv_setpvs
  //  LiteralArgNum_ - which argument (ignoring aTHX) in the final call
  //    to the underlying Perl_ prefix function is the literal string
  //  LengthArgNum_ - which argument (ignoring aTHX) in the final call
  //    to the underlying the Perl_ prefix function is the length of the
  //    string.
  //  KeepArgs_ - int vector of arguments from the call to the Perl_
  //    prefix function end up in the FixItHint replacement
  //
  PerlLiteralFunctionCheck(llvm::StringRef Name,
                           clang::tidy::ClangTidyContext* Context,
                           llvm::StringRef PvnMacro_, llvm::StringRef PvsMacro_,
                           int LiteralArgNum_, int LengthArgNum_,
                           std::vector<int> &&KeepArgs_);
  void registerMatchers(clang::ast_matchers::MatchFinder* Finder) override;
  void check(const clang::ast_matchers::MatchFinder::MatchResult& Result) override;
};
