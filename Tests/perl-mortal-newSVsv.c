#include "perlsim.h"

extern SV *Perl_newSVsv_flags(pTHX_ SV * const old, int flags);
extern SV *Perl_sv_mortalcopy_flags(pTHX_ SV * oldsv, unsigned flags);
#define newSVsv(sv) Perl_newSVsv_flags(aTHX_ (sv), SV_GMAGIC|SV_NOSTEAL)
#define sv_mortalcopy(sv) \
        Perl_sv_mortalcopy_flags(aTHX_ sv, SV_GMAGIC|SV_DO_COW_SVSETSV)

extern SV *Perl_sv_2mortal(pTHX_ SV *sv);
#define sv_2mortal(sv) Perl_sv_2mortal(aTHX_ sv)

extern SV *my_sv;

void foo(pTHX_ SV *sv) {
  my_sv = sv_2mortal(newSVsv(sv));
}
