#include "perlsim.h"

extern SV *Perl_newSVpvn(pTHX_ const char *s, size_t len);
#define newSVpvn(s, len) Perl_newSVpvn(aTHX_ s, len)
#define newSVpvs(s) Perl_newSVpvn(s, sizeof(s "")-1)

extern SV *my_sv;

void foo(pTHX) {
  my_sv = newSVpvs("Foo");
  my_sv = newSVpvs("foo");
  my_sv = newSVpvn(buf, buflen);
  my_sv = newSVpvn("foo", 2);
  my_sv = newSVpvn("foo", 4);
  my_sv = newSVpvs(WARNbits);
  my_sv = newSVpvs("foo");
}
