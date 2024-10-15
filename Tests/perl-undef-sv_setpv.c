#include "perlsim.h"

extern void Perl_sv_setpv(pTHX_ SV *dsv, const char *s);
#define sv_setpv(dsv, s) Perl_sv_setpv(aTHX_ dsv, s)
extern void Perl_sv_set_undef(pTHX_ SV *dsv);
#define sv_set_undef(dsv) Perl_sv_set_undef(aTHX_ dsv)

void foo(pTHX_ SV *sv) {
  sv_setpv(sv, buf);
  sv_setpv(sv, NULL);
  sv_setpv(sv, 0);
  sv_setpv(sv, "Hello");
  sv_set_undef(sv);
}
