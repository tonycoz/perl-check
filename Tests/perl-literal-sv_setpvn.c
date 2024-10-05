#include "perlsim.h"

extern void Perl_sv_setpvn(pTHX_ SV *sv, const char *s, size_t len);
#define sv_setpvn(sv, s, len) Perl_sv_setpvn(aTHX_ sv, s, len)
#define sv_setpvs(sv, s) Perl_sv_setpvn(aTHX_ sv, s "", sizeof(s "")-1)

void foo(pTHX_ SV *sv) {
  sv_setpvn(sv, "Foo", 3);
  sv_setpvn(sv, "foo", sizeof("foo")-1);
  sv_setpvn(sv, buf, buflen);
  sv_setpvn(sv, "foo", 2);
  sv_setpvn(sv, "foo", 4);
  sv_setpvn(sv, WARNbits, WARNbits_size);
  sv_setpvs(sv, "foo");
}
