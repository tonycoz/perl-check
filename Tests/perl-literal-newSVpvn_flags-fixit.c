#include "perlsim.h"

extern SV *Perl_newSVpvn_flags(pTHX_ const char *s, size_t len, unsigned flags);
#define newSVpvn_flags(s, len, flags) Perl_newSVpvn_flags(aTHX_ s, len, flags)
#define newSVpvs_flags(s, flags) Perl_newSVpvn_flags(s, sizeof(s "")-1, flags)

extern SV *my_sv;

void foo(pTHX) {
  my_sv = newSVpvs_flags("Foo", SVs_TEMP);
  my_sv = newSVpvs_flags("foo", SVs_TEMP);
  my_sv = newSVpvn_flags(buf, buflen, SVs_TEMP);
  my_sv = newSVpvn_flags("foo", 2, SVs_TEMP);
  my_sv = newSVpvn_flags("foo", 4, SVs_TEMP);
  my_sv = newSVpvs_flags(WARNbits, SVs_TEMP);
  my_sv = newSVpvs_flags("foo", SVs_TEMP);
}
