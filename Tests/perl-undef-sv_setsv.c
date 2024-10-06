#include "perlsim.h"

extern void Perl_sv_setsv(pTHX_ SV *dsv, SV *ssv);
#define sv_setsv(dsv, ssv) Perl_sv_setsv(aTHX_ dsv, ssv)
extern void Perl_sv_set_undef(pTHX_ SV *dsv);
#define sv_set_undef(dsv) Perl_sv_set_undef(aTHX_ dsv)

void foo(pTHX_ SV *sv) {
  sv_setsv(sv, &PL_sv_undef);
  sv_set_undef(sv);
}
