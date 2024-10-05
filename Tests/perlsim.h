#include <stddef.h>

#ifdef MULTIPLICITY
struct PerlInterpreter;
#define pTHX struct PerlInterpreter *my_perl
#define pTHX_ pTHX_
#define aTHX my_perl
#define aTHX_ aTHX,
#else
#define aTHX void
#define aTHX_
#define pTHX void
#define pTHX_
#endif

typedef struct sv SV;

extern SV *my_sv;

char buf[10];
int buflen;

#define WARNbits "1234"
#define WARNbits_size 4

#define SVs_TEMP 0x01
