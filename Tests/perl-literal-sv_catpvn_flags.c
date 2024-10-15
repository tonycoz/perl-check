#include "perlsim.h"

extern void Perl_sv_catpvn_flags(pTHX_ SV *sv, const char *s, size_t len, int flags);
#define sv_catpvn_flags(sv, s, len, flags) Perl_sv_catpvn_flags(aTHX_ sv, s, len, flags)
#define sv_catpvs_flags(sv, s, flags) Perl_sv_catpvn_flags(aTHX_ sv, s "", sizeof(s "")-1, flags)

void foo(pTHX_ SV *sv) {
  sv_catpvn_flags(sv, "Foo", 3, SV_CATBYTES);
  sv_catpvn_flags(sv, "foo", sizeof("foo")-1, SV_CATBYTES);
  sv_catpvn_flags(sv, buf, buflen, SV_CATBYTES);
  sv_catpvn_flags(sv, "foo", 2, SV_CATBYTES);
  sv_catpvn_flags(sv, "foo", 4, SV_CATBYTES);
  sv_catpvn_flags(sv, WARNbits, WARNbits_size, SV_CATBYTES);
  sv_catpvs_flags(sv, "foo", SV_CATBYTES);
}
