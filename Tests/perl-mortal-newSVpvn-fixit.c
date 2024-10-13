#include "perlsim.h"

extern SV *Perl_newSVpvn(pTHX_ const char *s, size_t len);
#define newSVpvn(s, len) Perl_newSVpvn(aTHX_ s, len)
#define newSVpvs(s) Perl_newSVpvn(s, sizeof(s "")-1)

extern SV *Perl_sv_2mortal(pTHX_ SV *sv);
#define sv_2mortal(sv) Perl_sv_2mortal(aTHX_ sv)

extern SV *my_sv;

void foo(pTHX) {
  my_sv = newSVpvn_flags(buf, buflen, SVs_TEMP);
}
