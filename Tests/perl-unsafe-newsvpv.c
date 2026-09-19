#include "perlsim.h"
#include <string.h>

extern SV *Perl_newSVpv(pTHX_ const char *s, size_t len);
#define newSVpv(s, len) Perl_newSVpv(aTHX_ s, len)

extern SV *my_sv;
extern char *some_str;
extern size_t some_len;

void foo(pTHX) {
  my_sv = newSVpv(some_str, 0);
  my_sv = newSVpv(some_str, 1);
  my_sv = newSVpv(some_str, some_len);
  my_sv = newSVpv("abc", some_len);
  my_sv = newSVpv(some_str, strlen(some_str));
}
