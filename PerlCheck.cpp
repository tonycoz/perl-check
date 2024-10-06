#include "PerlLiteralFunctionCheck.h"
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
                1, 2, std::vector{ 0, 1 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-newSVpvn",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "newSVpvn", "newSVpvs",
                0, 1, std::vector{ 0 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-newSVpvn_flags",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "newSVpvn_flags", "newSVpvs_flags",
                0, 1, std::vector{ 0, 2 });
          });
      CheckFactories.registerCheckFactory(
          "perl-literal-hv_fetch",
          [](llvm::StringRef Name, ClangTidyContext *Context){
            return std::make_unique<PerlLiteralFunctionCheck>(
                Name, Context, "hv_fetch", "hv_fetchs",
                1, 2, std::vector{ 0, 1, 3 });
          });
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
