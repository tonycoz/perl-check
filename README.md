# perl-check

This is a clang-tidy plugin that currently checks for calls to
`sv_setpvn()` with a literal string, which are better written with
`sv_setpvs()`.

# Building the module

Install clang and its development libraries, I used the debs from
apt.llvm.org, and tested with llvm 18.

You also need cmake, you may also want libedit-dev.
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

## For your perl build

Switch to your perl checkout, you need to start with a clean tree.

This requires the `bear` tool, I used the Debian packaged version.

```
./Configure ... -Dcc=clang-18
bear -- make
```
You should now have `compile-commands.json`

## For an XS module

Switch to your XS root, assuming an `ExtUtils::MakeMaker` build:
```
perl Makefile.PL
bear -- make
```

# Invoking clang-tidy

```
clang-tidy-18 --load=path/to/build/libperl-check.so --checks='-*,perl-*' --config='{"CheckOptions":{"PerlCheckMultiplicity":1}}' source.c
```

The `clang-tidy` invoked should match the version the loadable module
was built with, the simplest way to see that is:

```
$ grep Clang_DIR build/CMakeCache.txt
Clang_DIR:PATH=/usr/lib/cmake/clang-18
```

You can also use `run-clang-tidy` to avoid test all sources, or all
sources matching a regular expression:

```
run-clang-tidy-18 -load=path/to/build/libperl-check.so -checks='-*,perl-*' -config='{"CheckOptions":{"PerlCheckMultiplicity":1}}'
```
Note the change from `--` to `-` for the options.

You will need to supply `PerlCheckMultiplicity` set to 1 as in the
above examples if your perl was built with threads, or even just
multiplicity.

# Example output

Edited out the files with no issues found to reduce noise.

```
$ run-clang-tidy-18 -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so -checks='-*,perl-*' -config='{"CheckOptions":{"PerlCheckMultiplicity":1}}' 'perl6/[a-z_]+\.c'
Enabled checks:
    perl-literal-get_cvn_flags
    perl-literal-hv_fetch
    perl-literal-newSVpvn
    perl-literal-newSVpvn_flags
    perl-literal-savepvn
    perl-literal-sv_catpvn
    perl-literal-sv_catpvn_flags
    perl-literal-sv_catpvn_nomg
    perl-literal-sv_setpvn
    perl-literal-sv_setpvn_mg
    perl-mortal-newSVpvn
    perl-mortal-newSVsv
    perl-undef-sv_setpv
    perl-undef-sv_setpvn
    perl-undef-sv_setsv

...
mro_core.c:1195:39: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 1195 |                             subname = sv_2mortal(newSVsv(namesv));
      |                                       ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
mro_core.c:1278:39: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 1278 |                             subname = sv_2mortal(newSVsv(namesv));
      |                                       ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
4 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/hv.c
hv.c:701:29: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
  701 |                     keysv = sv_2mortal(newSVsv(keysv));
      |                             ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/gv.c
gv.c:2200:29: warning: sv_setsv(..., &PL_sv_undef) better written as sv_set_undef() [perl-undef-sv_setsv]
 2200 |                             sv_setsv(*init, &PL_sv_undef);
      |                             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                             sv_set_undef(*init)
./embed.h:804:49: note: expanded from macro 'sv_setsv'
  804 | # define sv_setsv(a,b)                          Perl_sv_setsv(aTHX,a,b)
      |                                                 ^
./sv.h:2274:9: note: expanded from macro 'Perl_sv_setsv'
 2274 |         Perl_sv_setsv_flags(aTHX_ dsv, ssv, SV_GMAGIC|SV_DO_COW_SVSETSV)
      |         ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/dump.c
dump.c:3109:15: warning: newSVpvn_flags() with literal better written as newSVpvs_flags() [perl-literal-newSVpvn_flags]
 3109 |     SV *out = newSVpvn_flags("",0,SVs_TEMP);
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |               newSVpvs_flags("", SVs_TEMP)
./embed.h:468:49: note: expanded from macro 'newSVpvn_flags'
  468 | # define newSVpvn_flags(a,b,c)                  Perl_newSVpvn_flags(aTHX_ a,b,c)
      |                                                 ^
dump.c:3252:15: warning: newSVpvn_flags() with literal better written as newSVpvs_flags() [perl-literal-newSVpvn_flags]
 3252 |     SV *out = newSVpvn_flags("", 0, SVs_TEMP);
      |               ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |               newSVpvs_flags("", SVs_TEMP)
./embed.h:468:49: note: expanded from macro 'newSVpvn_flags'
  468 | # define newSVpvn_flags(a,b,c)                  Perl_newSVpvn_flags(aTHX_ a,b,c)
      |                                                 ^
4 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/pp.c
pp.c:3405:26: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 3405 |          SV* const tsv = sv_2mortal(newSVsv(sv));
      |                          ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/toke.c
toke.c:1891:25: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 1891 |             SV *name2 = sv_2mortal(newSVsv(PL_curstname));
      |                         ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/pp_hot.c
pp_hot.c:5498:24: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 5498 |                 TARG = sv_2mortal(newSVsv(TARG));
      |                        ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/mg.c
mg.c:1135:17: warning: sv_setpvn() with literal better written as sv_setpvs() [perl-literal-sv_setpvn]
 1135 |                 sv_setpvn(sv, WARN_NONEstring, WARNsize) ;
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                 sv_setpvs(sv, WARN_NONEstring)
./embed.h:792:49: note: expanded from macro 'sv_setpvn'
  792 | # define sv_setpvn(a,b,c)                       Perl_sv_setpvn(aTHX_ a,b,c)
      |                                                 ^
mg.c:1141:17: warning: sv_setpvn() with literal better written as sv_setpvs() [perl-literal-sv_setpvn]
 1141 |                 sv_setpvn(sv, WARN_ALLstring, WARNsize);
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                 sv_setpvs(sv, WARN_ALLstring)
./embed.h:792:49: note: expanded from macro 'sv_setpvn'
  792 | # define sv_setpvn(a,b,c)                       Perl_sv_setpvn(aTHX_ a,b,c)
      |                                                 ^
4 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/op.c
op.c:4065:26: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 4065 |                 svname = sv_2mortal(newSVsv(PL_curstname));
      |                          ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/universalmini.c
universalmini.c:1107:23: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 1107 |             pattern = sv_2mortal(newSVsv(MUTABLE_SV(re)));
      |                       ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/universal.c
universal.c:1107:23: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 1107 |             pattern = sv_2mortal(newSVsv(MUTABLE_SV(re)));
      |                       ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/pp_ctl.c
pp_ctl.c:1444:22: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 1444 |                 sv = sv_2mortal(newSVsv(sv));
      |                      ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
pp_ctl.c:2242:23: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 2242 |                 PUSHs(sv_2mortal(newSVsv(cur_text)));
      |                       ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
pp_ctl.c:2249:13: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 2249 |             mPUSHs(newSVsv(cx->blk_eval.old_namesv));
      |             ^
./pp.h:567:25: note: expanded from macro 'mPUSHs'
  567 | #define mPUSHs(s)       PUSHs(sv_2mortal(s))
      |                               ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
pp_ctl.c:2269:20: warning: newSVpvn() with literal better written as newSVpvs() [perl-literal-newSVpvn]
 2269 |             mask = newSVpvn(WARN_NONEstring, WARNsize) ;
      |                    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                    newSVpvs(WARN_NONEstring)
./embed.h:467:49: note: expanded from macro 'newSVpvn'
  467 | # define newSVpvn(a,b)                          Perl_newSVpvn(aTHX_ a,b)
      |                                                 ^
pp_ctl.c:2274:20: warning: newSVpvn() with literal better written as newSVpvs() [perl-literal-newSVpvn]
 2274 |             mask = newSVpvn(WARN_ALLstring, WARNsize) ;
      |                    ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                    newSVpvs(WARN_ALLstring)
./embed.h:467:49: note: expanded from macro 'newSVpvn'
  467 | # define newSVpvn(a,b)                          Perl_newSVpvn(aTHX_ a,b)
      |                                                 ^
7 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/sv.c
sv.c:6693:23: warning: newSVpvn_flags() with literal better written as newSVpvs_flags() [perl-literal-newSVpvn_flags]
 6693 |                     : newSVpvn_flags( "__ANON__", 8, 0 );
      |                       ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                       newSVpvs_flags("__ANON__", 0)
./embed.h:468:49: note: expanded from macro 'newSVpvn_flags'
  468 | # define newSVpvn_flags(a,b,c)                  Perl_newSVpvn_flags(aTHX_ a,b,c)
      |                                                 ^
sv.c:17586:18: warning: newSVpvn_flags() with literal better written as newSVpvs_flags() [perl-literal-newSVpvn_flags]
 17586 |             sv = newSVpvn_flags("", 0, SVs_TEMP);
       |                  ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
       |                  newSVpvs_flags("", SVs_TEMP)
./embed.h:468:49: note: expanded from macro 'newSVpvn_flags'
  468 | # define newSVpvn_flags(a,b,c)                  Perl_newSVpvn_flags(aTHX_ a,b,c)
      |                                                 ^
4 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/builtin.c
builtin.c:560:9: warning: sv_setsv(..., &PL_sv_undef) better written as sv_set_undef() [perl-undef-sv_setsv]
  560 |         sv_setsv(TARG, &PL_sv_undef);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         sv_set_undef(TARG)
./embed.h:804:49: note: expanded from macro 'sv_setsv'
  804 | # define sv_setsv(a,b)                          Perl_sv_setsv(aTHX,a,b)
      |                                                 ^
./sv.h:2274:9: note: expanded from macro 'Perl_sv_setsv'
 2274 |         Perl_sv_setsv_flags(aTHX_ dsv, ssv, SV_GMAGIC|SV_DO_COW_SVSETSV)
      |         ^
builtin.c:576:9: warning: sv_setsv(..., &PL_sv_undef) better written as sv_set_undef() [perl-undef-sv_setsv]
  576 |         sv_setsv(TARG, &PL_sv_undef);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         sv_set_undef(TARG)
./embed.h:804:49: note: expanded from macro 'sv_setsv'
  804 | # define sv_setsv(a,b)                          Perl_sv_setsv(aTHX,a,b)
      |                                                 ^
./sv.h:2274:9: note: expanded from macro 'Perl_sv_setsv'
 2274 |         Perl_sv_setsv_flags(aTHX_ dsv, ssv, SV_GMAGIC|SV_DO_COW_SVSETSV)
      |         ^
4 warnings generated.
Suppressed 2 warnings (2 with check filters).
...
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/opmini.c
opmini.c:4065:26: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 4065 |                 svname = sv_2mortal(newSVsv(PL_curstname));
      |                          ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
3 warnings generated.
Suppressed 2 warnings (2 with check filters).
clang-tidy-18 -checks=-*,perl-* -p=/home/tony/dev/perl/git/perl6 -config={"CheckOptions":{"PerlCheckMultiplicity":1}} -load=/home/tony/dev/llvm/git/perl-check/build/libperl-check.so /home/tony/dev/perl/git/perl6/regcomp.c
regcomp.c:809:27: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
  809 |                     pat = sv_2mortal(newSVsv(msv));
      |                           ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
regcomp.c:10711:33: warning: newSVpvn_flags() with literal better written as newSVpvs_flags() [perl-literal-newSVpvn_flags]
 10711 |         SV * substitute_parse = newSVpvn_flags("?:", 2, SVs_TEMP);
       |                                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
       |                                 newSVpvs_flags("?:", SVs_TEMP)
./embed.h:468:49: note: expanded from macro 'newSVpvn_flags'
  468 | # define newSVpvn_flags(a,b,c)                  Perl_newSVpvn_flags(aTHX_ a,b,c)
      |                                                 ^
regcomp.c:16069:19: warning: sv_2mortal(newSVsv(...)) better written as sv_mortalcopy() [perl-mortal-newSVsv]
 16069 |         must_sv = sv_2mortal(newSVsv(must_sv));
       |                   ^
./embed.h:686:49: note: expanded from macro 'sv_2mortal'
  686 | # define sv_2mortal(a)                          Perl_sv_2mortal(aTHX_ a)
      |                                                 ^
5 warnings generated.
Suppressed 2 warnings (2 with check filters).
```

# Checks

* `perl-literal-sv_setpvn`
* `perl-literal-sv_setpvn_mg`
* `perl-literal-hv_fetch`
* `perl-literal-newSVpvn`
* `perl-literal-newSVpvn_flags`
* `perl-literal-sv_catpvn`
* `perl-literal-sv_catpvn_flags`
* `perl-literal-sv_catpvn_nomg`
* `perl-literal-savepvn`
* `perl-literal-get_cvn_flags`

If any of the corresponding API macros is called with a string literal
and the length of that literal, suggest the corresponding API that
calculates the length automatically.

* `perl-undef-sv_setsv`

Complain about using `sv_setsv(sv, &PL_sv_undef)` since
`sv_set_undef(sv)` is likely faster.

* `perl-undef-setpv`
* `perl-undef-setpvn`

Complain about `sv_setpv(sv, NULL)` or `sv_setpvn(sv, NULL, 0)` to set
`sv` to `undef`, and suggest/FIXIT `sv_set_undef(sv)`.

* `perl-mortal-newSVpvn`

Check for calls to the given function wrapped by `sv_2mortal()`, when
the wrapped function has a flag to mortalize the result.

* `perl-mortal-newSVsv`

Check for calls to the given function wrapped by `sv_2mortal()`, when
`sv_mortalcopy()` does the same *and* mortalizes.

The `newSVsv()` check does not produce a fix it (I tried, but it
wouldn't replace, I suspect the macros confused clang-tidy.)

# FixIts

Most checks provide fixits, and clang-tidy can apply the fixes.
