#include <stddef.h>

#ifdef MULTIPLICITY
struct PerlInterpreter;
struct PerlInterpreter *my_perl;
#define pTHX_ struct PerlInterpreter *my_perl,
#define aTHX_ my_perl,
#else
#define aTHX_
#define pTHX_
#endif

typedef struct sv SV;

extern SV *my_sv;

char buf[10];
int buflen;

#define WARNbits "1234"
#define WARNbits_size 4
