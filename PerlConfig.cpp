#include "PerlConfig.h"

#include <llvm/ADT/StringRef.h>
#include <optional>
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/FileUtilities.h"
#include "llvm/Support/Program.h"

using namespace llvm;

[[nodiscard]] static bool is_file(const char *path)
{
  using namespace llvm::sys::fs;
  file_status st;
  if (!status(path, st)) {
    return is_regular_file(st);
  }
  else {
    return false;
  }
}

template <typename T>
raw_ostream &
operator <<(raw_ostream &out, const ArrayRef<T> &arr) {
  int first = true;
  for (auto &item : arr) {
    if (!first)
      out << ' ';
    out << item;
    first = false;
  }
  return out;
}

[[nodiscard]] static int ExecuteAndCapture
(StringRef Executable, ArrayRef<StringRef> Args,
 std::unique_ptr<MemoryBuffer>& Output) {
  SmallString<64> OutputFile;
  if (llvm::sys::fs::createTemporaryFile("perl", "txt", OutputFile)) {
          return -1;
  }
  FileRemover OutputRemover(OutputFile.c_str());
  std::optional<StringRef> inout[] = {
    {""},
    {OutputFile.str()},
    std::nullopt
  };
  int result = llvm::sys::ExecuteAndWait(Executable, Args, std::nullopt,
                                         inout);
  if (result == 0) {
    ErrorOr<std::unique_ptr<MemoryBuffer>> OutputBuf =
      MemoryBuffer::getFile(OutputFile.c_str());
    if (OutputBuf) {
      Output = std::move(*OutputBuf);
    }
  }
  return result;
}

PerlConfig::PerlConfig(clang::tidy::ClangTidyCheck::OptionsView &Options) {
  // <bool> isn't exported and it's definition isn't visible
  auto mult = Options.getLocalOrGlobal<int>("PerlCheckMultiplicity");
  if (mult) {
    UseMultiplicity = *mult;
    errs() << "perl-check: usemultiplicity="
           << (UseMultiplicity ? "define" : "undef")
           << " from configuration\n";
  }
  else {
    UseMultiplicity = false;
    auto perl = Options.getLocalOrGlobal("PerlBin");
    std::string work;
    StringRef args[3];
    size_t count = 1;
    if (!perl) {
      if (is_file("perl")) {
        perl = { "./perl" };
        args[1] = "-Ilib";
        ++count;
      }
      else {
        auto found = llvm::sys::findProgramByName("perl");
        if (found) {
          work = *found;
          perl = { static_cast<llvm::StringRef>(work) };
        }
      }
    }
    if (perl) {
      args[0] = *perl;
      args[count++] = {"-V::usemultiplicity:"};
      ArrayRef<StringRef> argsArr{args, count};
      std::unique_ptr<MemoryBuffer> Output;
      if (ExecuteAndCapture(*perl, argsArr, Output) == 0 &&
          Output) {
        // should be "'define' " or "'undef' "
        UseMultiplicity = Output->getBuffer()[1] == 'd';
        errs() << "perl-check: usemultiplicity="
               << (UseMultiplicity ? "define" : "undef")
               << " from \""
               << argsArr
               << "\"\n";
      }
      else {
        errs() << "perl-check: failed to execute \""
               << argsArr
               << "\", assuming usemultiplicity=undef\n";
      }
    }
    else {
      errs() << "perl-check: no perl found, assuming usemultiplicity=undef\n";
    }
  }
}
