#include "PerlCheck.h"
#include "PerlLiteralFunctionCheck.h"
#include "PerlUndefSetsvCheck.h"
#include "PerlUndefSetpvXCheck.h"
#include "PerlMortalFunctionCheck.h"
#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;


namespace {

class PerlCheckModule : public ClangTidyModule
{
public:
    void addCheckFactories(ClangTidyCheckFactories& CheckFactories) override
    {
      CheckFactories.registerCheckFactory(
          "perl-literal-sv_setpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "sv_setpvn", "sv_setpvs",
                1, 2, llvm::SmallVector{ 0, 1 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-sv_setpvn_mg",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "sv_setpvn_mg", "sv_setpvs_mg",
                1, 2, llvm::SmallVector{ 0, 1 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-newSVpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "newSVpvn", "newSVpvs",
                0, 1, llvm::SmallVector{ 0 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-newSVpvn_flags",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "newSVpvn_flags", "newSVpvs_flags",
                0, 1, llvm::SmallVector{ 0, 2 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-hv_fetch",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "hv_fetch", "hv_fetchs",
                1, 2, llvm::SmallVector{ 0, 1, 3 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-sv_catpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "sv_catpvn", "sv_catpvs",
                1, 2, llvm::SmallVector{ 0, 1 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-sv_catpvn_flags",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "sv_catpvn_flags", "sv_catpvs_flags",
                1, 2, llvm::SmallVector{ 0, 1, 3 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-sv_catpvn_nomg",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "sv_catpvn_nomg", "sv_catpvs_nomg",
                1, 2, llvm::SmallVector{ 0, 1 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-savepvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "savepvn", "savepvs",
                0, 1, llvm::SmallVector{ 0 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-get_cvn_flags",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "get_cvn_flags", "get_cvs",
                0, 1, llvm::SmallVector{ 0, 2 });
          });

      CheckFactories.registerCheck<PerlUndefSetsvCheck>("perl-undef-sv_setsv");

      CheckFactories.registerCheckFactory(
          "perl-undef-sv_setpv",
          [](llvm::StringRef Name, ClangTidyContext *Context) {
            return std::make_unique<PerlUndefSetpvXCheck>(
                 Name, Context, "sv_setpv", "Perl_sv_setpv");
          });
      CheckFactories.registerCheckFactory(
          "perl-undef-sv_setpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context) {
            return std::make_unique<PerlUndefSetpvXCheck>(
                Name, Context, "sv_setpvn", "Perl_sv_setpvn");
          });

      CheckFactories.registerCheckFactory(
          "perl-mortal-newSVpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context) {
            return std::make_unique<PerlMortalFunctionCheck>(
                Name, Context, "newSVpvn", "newSVpvn_flags",
                -1, llvm::SmallVector{ 0, 1, -1 }, "SVs_TEMP"sv
                );
          });
#if 1
      CheckFactories.registerCheckFactory(
          "perl-mortal-newSVsv",
          [](llvm::StringRef Name, ClangTidyContext *Context) {
            return std::make_unique<PerlMortalFunctionCheck>(
                Name, Context, "newSVsv", "sv_mortalcopy_flags",
                -1, llvm::SmallVector{ 0, -1 }, "SV_GMAGIC|SV_NOSTEAL"sv, false
                );
          });
#endif
    }
};

}  // namespace

namespace clang::tidy {

// Register the module using this statically initialized variable.
static ClangTidyModuleRegistry::Add<::PerlCheckModule> perlCheckInit(
 "perl-check", "Add checks perl source checks.");

// This anchor is used to force the linker to link in the generated object file and thus register the module.
volatile int perlCheckAnchorSource = 0;

}  // namespace clang::tidy

namespace perl_check {

  using perl_check::GetArgs;

std::string
MakeCall(llvm::StringRef NewName, const GetArgs &Args,
          const llvm::SmallVector<int> &KeepArgs,
          int InFlagsArgNum,
          llvm::StringRef DefaultFlags, bool RealArgs) {
  std::string result;
  llvm::raw_string_ostream srepl{result};
  srepl.reserveExtraSpace(80); // typically enough
  srepl << NewName << "(";
  srepl << Args(KeepArgs[0]);
  for (auto i = std::next(KeepArgs.begin()); i != KeepArgs.end(); ++i) {
    srepl << ", ";
    if (*i == -1) {
      if (InFlagsArgNum == -1) {
        srepl << DefaultFlags;
      }
      else {
        if (RealArgs) {
          srepl << Args(*i);
        }
        else {
          srepl << "...";
        }
        srepl << " | " << DefaultFlags;
      }
    }
    else {
      if (RealArgs) {
        srepl << Args(*i);
      }
      else {
        srepl << "...";
      }
    }
  }
  srepl << ")";

  return result;
}

}
