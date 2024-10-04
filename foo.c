#include <stddef.h>

#ifdef MULTIPLICITY
struct PerlInterpreter;
struct PerlInterpreter *my_perl;
#define pTHX_ struct PerlInterpreter *my_perl,
#define aTHX_ my_perl,
#else
#define aTHX_
#define pTHX_
#endif

typedef struct sv SV;

extern void Perl_sv_setpvn(pTHX_ SV *sv, const char *s, size_t len);
#define sv_setpvn(sv, s, len) Perl_sv_setpvn(aTHX_ sv, s, len)
#define sv_setpvs(sv, s) Perl_sv_setpvn(aTHX_ sv, s "", sizeof(s "")-1)

extern SV *Perl_newSVpvn(pTHX_ const char *s, size_t len);
#define newSVpvn(s, len) Perl_newSVpvn(aTHX_ s, len)
#define newSVpvs(s) Perl_newSVpvn(s, sizeof(s "")-1)

char buf[10];
int buflen;

#define WARNbits "1234"
#define WARNbits_size 4

extern SV *my_sv;

void foo(pTHX_ SV *sv) {
  sv_setpvn(sv, "Foo", 3);
  sv_setpvn(sv, "foo", sizeof("foo")-1);
  sv_setpvn(sv, buf, buflen);
  sv_setpvn(sv, "foo", 2);
  sv_setpvn(sv, "foo", 4);
  sv_setpvn(sv, WARNbits, WARNbits_size);
  sv_setpvs(sv, "foo");

  my_sv = newSVpvn("ab", 2);
}

void awesome_foo() { }
