#include "perlsim.h"

void foo(SV *sv, AV *av, HV *hv, IO *io, GV *gv, char *cp, void *vp) {
  Safefree(sv);
  Safefree(av);
  Safefree(hv);
  Safefree(gv);
  Safefree(io);
  Safefree(cp);
  Safefree(vp);
}
