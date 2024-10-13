#include <stddef.h>

typedef struct sv { unsigned r; } SV;

#ifdef MULTIPLICITY
struct PerlInterpreter {
  SV Iimmortals[5];
};
#  define pTHX struct PerlInterpreter *my_perl
#  define pTHX_ pTHX,
#  define aTHX my_perl
#  define aTHX_ aTHX,
#  define PL_immortals (my_perl->Iimmortals)
#else
#  define aTHX void
#  define aTHX_
#  define pTHX void
#  define pTHX_
extern SV PL_immortals[5];
#endif

#define PL_sv_undef (PL_immortals[0])

extern SV *my_sv;

char buf[10];
int buflen;

#define WARNbits "1234"
#define WARNbits_size 4

// some value
#define SVs_TEMP 0x01
#define SVf_UTF8 0x02
