#include "perlsim.h"

extern void Perl_sv_setpvn_mg(pTHX_ SV *sv, const char *s, size_t len);
#define sv_setpvn_mg(sv, s, len) Perl_sv_setpvn_mg(aTHX_ sv, s, len)
#define sv_setpvs_mg(sv, s) Perl_sv_setpvn_mg(aTHX_ sv, s "", sizeof(s "")-1)

void foo(pTHX_ SV *sv) {
  sv_setpvs_mg(sv, "Foo");
  sv_setpvs_mg(sv, "foo");
  sv_setpvn_mg(sv, buf, buflen);
  sv_setpvn_mg(sv, "foo", 2);
  sv_setpvn_mg(sv, "foo", 4);
  sv_setpvs_mg(sv, WARNbits);
  sv_setpvs_mg(sv, "foo");
}
