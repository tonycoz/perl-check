#include "perlsim.h"

extern char *Perl_savepvn(pTHX_ const char *s, size_t len);
#define savepvn(s, len) Perl_savepvn(aTHX_ s, len)
#define savepvs(s) Perl_savepvn(aTHX_ s "", sizeof(s "")-1)

extern char *p;

void foo(pTHX) {
  p = savepvs("Foo");
  p = savepvs("foo");
  p = savepvn(buf, buflen);
  p = savepvn("foo", 2);
  p = savepvn("foo", 4);
  p = savepvs(WARNbits);
  p = savepvs("foo");
}
