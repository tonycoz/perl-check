#include <stddef.h>

#include "Tests/perlsim.h"

extern void Perl_sv_setpvn(pTHX_ SV *sv, const char *s, size_t len);
#define sv_setpvn(sv, s, len) Perl_sv_setpvn(aTHX_ sv, s, len)
#define sv_setpvs(sv, s) Perl_sv_setpvn(aTHX_ sv, s "", sizeof(s "")-1)

extern SV *Perl_newSVpvn(pTHX_ const char *s, size_t len);
#define newSVpvn(s, len) Perl_newSVpvn(aTHX_ s, len)
#define newSVpvs(s) Perl_newSVpvn(s, sizeof(s "")-1)

typedef struct hv HV;

extern SV **Perl_hv_fetch(pTHX_ HV *hv, const char *s, size_t len, int lval);
#define hv_fetch(hv, s, len, lval) Perl_hv_fetch(aTHX_ hv, s, len, lval)
#define hv_fetchs(sv, s, lval) Perl_hv_fetch(aTHX_ sv, s "", sizeof(s "")-1, lval)

extern void Perl_sv_setsv(pTHX_ SV *dsv, SV *ssv);
extern void Perl_sv_set_undef(pTHX_ SV *dsv);
#define sv_setsv(dsv, ssv) Perl_sv_setsv(aTHX_ dsv, ssv)
#define sv_set_undef(dsv) Perl_sv_set_undef(aTHX_ dsv)

char buf[10];
int buflen;

#define WARNbits "1234"
#define WARNbits_size 4

extern SV *my_sv;

void foo(pTHX_ SV *sv, HV *hv) {
  sv_setpvn(sv, "Foo", 3);
  sv_setpvn(sv, "foo", sizeof("foo")-1);
  sv_setpvn(sv, buf, buflen);
  sv_setpvn(sv, "foo", 2);
  sv_setpvn(sv, "foo", 4);
  sv_setpvn(sv, WARNbits, WARNbits_size);
  sv_setpvs(sv, "foo");

  my_sv = newSVpvn("ab", 2);

  SV **svp = hv_fetch(hv, "Foo", 3, 1);

  sv_setsv(sv, &PL_sv_undef);
  sv_set_undef(sv);
}

void awesome_foo() { }
