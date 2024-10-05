perl-check
==========

This is a clang-tidy plugin that currently checks for calls to
sv_setpvn() with a literal string, which are better written with
sv_setpvs().

Building the module
========

Install clang and it's development libraries, I used the debs from
apt.llvm.org, and tested with llvm 18.

You also need cmake, you make also want libedit-dev.
```
cmake -B build
cmake --build build
```

which should create `libperl-check.so` in `build/`.  You'll need the
path to this and to `.perl-clang-tidy` in this directory.

If you have multiple versions of `clang` installed `cmake` seems to
select the first `clang` it finds going by directory order, you can
control that with `CMAKE_PREFIX_PATH`, for example:

```
export CMAKE_PREFIX_PATH=/usr/lib/llvm-20
cmake -B build
cmake --build build
```

Build a compilation database
===

Switch to your perl checkout, you need to start with a clean tree.

This requires the `bear` tool, I used the Debian packaged version.

```
./Configure ... -Dcc=clang-18
bear -- make
```
You should now have `compile-commands.json`

Invoking clang-tidy
======

```
clang-tidy-18 --load=path/to/build/libperl-check.so --config-file path/to/perl-check/.perl-clang-tidy source.c
```

The `clang-tidy` invoked should match the version the loadable module
was built with, the simplest way to see that is:

```
$ grep Clang_DIR build/CMakeCache.txt
Clang_DIR:PATH=/usr/lib/cmake/clang-18
```


Example output
========
```
SysV.c:609:7: warning: sv_setpvn() with literal better written as sv_setpvs() [perl-tidy-literal-functions]
  609 |       sv_setpvn(sv, "", 0);
      |       ^
../../embed.h:734:49: note: expanded from macro 'sv_setpvn'
  734 | # define sv_setpvn(a,b,c)                       Perl_sv_setpvn(aTHX_ a,b,c)
      |                                                 ^
```
Though this particular case could be using SvPVCLEAR().
