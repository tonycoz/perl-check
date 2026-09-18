#include "perlsim.h"

extern void Perl_sv_setpvn(pTHX_ SV *sv, const char *s, size_t len);
#define sv_setpvn(sv, s, len) Perl_sv_setpvn(aTHX_ sv, s, len)
#define sv_setpvs(sv, s) Perl_sv_setpvn(aTHX_ sv, s "", sizeof(s "")-1)

void foo(pTHX_ SV *sv) {
  SvPVCLEAR(sv);
}
