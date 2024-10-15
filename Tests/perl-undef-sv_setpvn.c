#include "perlsim.h"

extern void Perl_sv_setpvn(pTHX_ SV *dsv, const char *s, size_t len);
#define sv_setpvn(dsv, s, len) Perl_sv_setpvn(aTHX_ dsv, s, len)
extern void Perl_sv_set_undef(pTHX_ SV *dsv);
#define sv_set_undef(dsv) Perl_sv_set_undef(aTHX_ dsv)

void foo(pTHX_ SV *sv) {
  sv_setpvn(sv, buf, buflen);
  sv_setpvn(sv, NULL, 0);
  sv_setpvn(sv, 0, 0);
  sv_setpvn(sv, "Hello", 5);
  sv_set_undef(sv);
}
