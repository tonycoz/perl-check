#include "perlsim.h"

extern void Perl_sv_catpvn(pTHX_ SV *sv, const char *s, size_t len);
#define sv_catpvn(sv, s, len) Perl_sv_catpvn(aTHX_ sv, s, len)
#define sv_catpvs(sv, s) Perl_sv_catpvn(aTHX_ sv, s "", sizeof(s "")-1)

void foo(pTHX_ SV *sv) {
  sv_catpvs(sv, "Foo");
  sv_catpvs(sv, "foo");
  sv_catpvn(sv, buf, buflen);
  sv_catpvn(sv, "foo", 2);
  sv_catpvn(sv, "foo", 4);
  sv_catpvs(sv, WARNbits);
  sv_catpvs(sv, "foo");
}
