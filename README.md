# perl-check

This is a clang-tidy plugin that currently checks for calls to
sv_setpvn() with a literal string, which are better written with
sv_setpvs().

# Building the module

Install clang and its development libraries, I used the debs from
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

# Build a compilation database

Switch to your perl checkout, you need to start with a clean tree.

This requires the `bear` tool, I used the Debian packaged version.

```
./Configure ... -Dcc=clang-18
bear -- make
```
You should now have `compile-commands.json`

# Invoking clang-tidy

```
clang-tidy-18 --load=path/to/build/libperl-check.so --config-file path/to/perl-check/.perl-clang-tidy source.c
```

The `clang-tidy` invoked should match the version the loadable module
was built with, the simplest way to see that is:

```
$ grep Clang_DIR build/CMakeCache.txt
Clang_DIR:PATH=/usr/lib/cmake/clang-18
```


# Example output

```
$ run-clang-tidy-18 -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so -checks='-*,perl-*' builtin.c mro.c
Enabled checks:
    perl-literal-hv_fetch
    perl-literal-newSVpvn
    perl-literal-newSVpvn_flags
    perl-literal-sv_setpvn
    perl-undef-sv_setsv

clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/ext/mro/mro.c
mro.c:550:15: warning: newSVpvn_flags() with literal better written as newSVpvs_flags() [perl-literal-newSVpvn_flags]
  550 |       ST(0) = newSVpvn_flags("dfs", 3, SVs_TEMP);
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |               newSVpvs_flags("dfs", SVs_TEMP)
../../embed.h:440:49: note: expanded from macro 'newSVpvn_flags'
  440 | # define newSVpvn_flags(a,b,c)                  Perl_newSVpvn_flags(aTHX_ a,b,c)
      |                                                 ^
1 warning generated.
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/builtin.c
builtin.c:556:9: warning: sv_setsv(..., &PL_sv_undef) better written as sv_set_undef() [perl-undef-sv_setsv]
  556 |         sv_setsv(TARG, &PL_sv_undef);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         sv_set_undef(TARG)
./sv.h:2242:9: note: expanded from macro 'sv_setsv'
 2242 |         sv_setsv_flags(dsv, ssv, SV_GMAGIC|SV_DO_COW_SVSETSV)
      |         ^
./embed.h:747:49: note: expanded from macro 'sv_setsv_flags'
  747 | # define sv_setsv_flags(a,b,c)                  Perl_sv_setsv_flags(aTHX_ a,b,c)
      |                                                 ^
builtin.c:572:9: warning: sv_setsv(..., &PL_sv_undef) better written as sv_set_undef() [perl-undef-sv_setsv]
  572 |         sv_setsv(TARG, &PL_sv_undef);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         sv_set_undef(TARG)
./sv.h:2242:9: note: expanded from macro 'sv_setsv'
 2242 |         sv_setsv_flags(dsv, ssv, SV_GMAGIC|SV_DO_COW_SVSETSV)
      |         ^
./embed.h:747:49: note: expanded from macro 'sv_setsv_flags'
  747 | # define sv_setsv_flags(a,b,c)                  Perl_sv_setsv_flags(aTHX_ a,b,c)
      |                                                 ^
2 warnings generated.
```

# Checks

* `perl-literal-sv_setpvn`
* `perl-literal-hv_fetch`
* `perl-literal-newSVpvn`
* `perl-literal-newSVpvn_flags`

If any of the corresponding API macros is called with a string literal
and the length of that literal, suggest the corresponding API that
calculates the length automatically.

* `perl-undef-sv_setsv`

Complain about using `sv_setsv(sv, &PL_sv_undef)` since
`sv_set_undef(sv)` is likely faster.

* `perl-mortal-newSVpvn`

Check for calls to the given function wrapped by `sv_2mortal()`, when
the wrapped function has a flag to mortalize the result.

# FixIts

Currently all the checks provide fixits, and clang-tidy can apply the
fixes.
