#include "perlsim.h"

extern void Perl_sv_catpvn(pTHX_ SV *sv, const char *s, size_t len);
#define sv_catpvn(sv, s, len) Perl_sv_catpvn(aTHX_ sv, s, len)
#define sv_catpvs(sv, s) Perl_sv_catpvn(aTHX_ sv, s "", sizeof(s "")-1)

void foo(pTHX_ SV *sv) {
  sv_catpvn(sv, "Foo", 3);
  sv_catpvn(sv, "foo", sizeof("foo")-1);
  sv_catpvn(sv, buf, buflen);
  sv_catpvn(sv, "foo", 2);
  sv_catpvn(sv, "foo", 4);
  sv_catpvn(sv, WARNbits, WARNbits_size);
  sv_catpvs(sv, "foo");
}
