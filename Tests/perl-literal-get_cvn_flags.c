#include "perlsim.h"

extern CV *Perl_get_cvn_flags(pTHX_ const char *s, size_t len, int flags);
#define get_cvn_flags(s, len, flags) Perl_get_cvn_flags(aTHX_ s, len, flags)
#define get_cvs(s, flags) Perl_get_cvn_flags(aTHX_ s "", sizeof(s ""), flags)

extern CV *my_cv;

void foo(pTHX) {
  my_cv = get_cvn_flags("func", 4, 0);
  my_cv = get_cvn_flags("func", sizeof("func")-1, 0);
  my_cv = get_cvn_flags(buf, buflen, GV_ADD);
  my_cv = get_cvn_flags("func", 3, GV_ADD);
  my_cv = get_cvs("bar", 0);
}
