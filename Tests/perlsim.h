#include <stddef.h>

typedef struct sv { unsigned r; } SV;
typedef struct cv { unsigned r; } CV;

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
#define SV_CATBYTES 0x100
#define SV_CATUTF8 0x200
#define SV_GMAGIC 0x400
#define SV_SMAGIC 0x800
#define SV_NOSTEAL 0x1000
#define SV_DO_COW_SVSETSV 0x2000

#define GV_ADD 1
