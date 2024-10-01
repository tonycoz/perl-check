#include <stddef.h>

typedef struct sv SV;

extern void Perl_sv_setpvn(SV *sv, const char *s, size_t len);
#define sv_setpvn(sv, s, len) Perl_sv_setpvn(sv, s, len)
#define sv_setpvs(sv, s) Perl_sv_setpvn(sv, s "", sizeof(s "")-1)

char buf[10];
int buflen;

#define WARNbits "1234"
#define WARNbits_size 4

void foo(SV *sv) {
  sv_setpvn(sv, "Foo", 3);
  sv_setpvn(sv, "foo", sizeof("foo")-1);
  sv_setpvn(sv, buf, buflen);
  sv_setpvn(sv, "foo", 2);
  sv_setpvn(sv, "foo", 4);
  sv_setpvn(sv, WARNbits, WARNbits_size);
  sv_setpvs(sv, "foo");
}

void awesome_foo() { }
