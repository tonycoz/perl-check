#include "perlsim.h"

typedef struct hv HV;

extern SV **Perl_hv_fetch(pTHX_ HV *hv, const char *s, size_t len, int lval);
#define hv_fetch(hv, s, len, lval) Perl_hv_fetch(aTHX_ hv, s, len, lval)
#define hv_fetchs(sv, s, lval) hv_fetch(sv, s "", sizeof(s "")-1, lval)

void foo(pTHX_ HV *hv) {
  SV **svp;
  svp = hv_fetchs(hv, "Foo", 0);
  svp = hv_fetchs(hv, "foo", 0);
  svp = hv_fetch(hv, buf, buflen, 1);
  svp = hv_fetch(hv, "foo", 2, 0);
  svp = hv_fetch(hv, "foo", 4, 0);
  svp = hv_fetchs(hv, WARNbits, 0);
  svp = hv_fetchs(hv, "foo", 1);
}
