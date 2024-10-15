#include "perlsim.h"

extern void Perl_sv_catpvn_flags(pTHX_ SV *sv, const char *s, size_t len, int flags);
#define sv_catpvn_nomg(sv, s, len) \
  Perl_sv_catpvn_flags(aTHX_ sv, s, len, 0)
#define sv_catpvs_nomg(sv, s) \
  Perl_sv_catpvn_flags(aTHX_ sv, s "", sizeof(s "")-1, 0)

void foo(pTHX_ SV *sv) {
  sv_catpvn_nomg(sv, "Foo", 3);
  sv_catpvn_nomg(sv, "foo", sizeof("foo")-1);
  sv_catpvn_nomg(sv, buf, buflen);
  sv_catpvn_nomg(sv, "foo", 2);
  sv_catpvn_nomg(sv, "foo", 4);
  sv_catpvn_nomg(sv, WARNbits, WARNbits_size);
  sv_catpvs_nomg(sv, "foo");
}
