#include "perlsim.h"

extern void Perl_sv_setpvn_mg(pTHX_ SV *sv, const char *s, size_t len);
#define sv_setpvn_mg(sv, s, len) Perl_sv_setpvn_mg(aTHX_ sv, s, len)
#define sv_setpvs_mg(sv, s) Perl_sv_setpvn_mg(aTHX_ sv, s "", sizeof(s "")-1)

void foo(pTHX_ SV *sv) {
  sv_setpvn_mg(sv, "Foo", 3);
  sv_setpvn_mg(sv, "foo", sizeof("foo")-1);
  sv_setpvn_mg(sv, buf, buflen);
  sv_setpvn_mg(sv, "foo", 2);
  sv_setpvn_mg(sv, "foo", 4);
  sv_setpvn_mg(sv, WARNbits, WARNbits_size);
  sv_setpvs_mg(sv, "foo");
}
