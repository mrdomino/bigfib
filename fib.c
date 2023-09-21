#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gmp.h>

typedef struct {
  mpz_t a, b, c, d;
} mat;

[[noreturn]] void usage(char const* argv0)
{
  fprintf(stderr, "usage: %s <n>\nfib 0.2 - gmp/%s\n", argv0, gmp_version);
  exit(1);
}

uint64_t read_n(char const* argv0, char const* arg)
{
  static_assert(sizeof(unsigned long long) == sizeof(uint64_t));
  unsigned long long n;
  char *end;

  while (isspace(*arg)) {
    ++arg;
  }
  if (*arg == '-') {
    usage(argv0);
  }
  n = strtoull(arg, &end, 0);
  if ((n == 0 && end == arg) ||
      (n == ULLONG_MAX && errno) ||
      *end) {
    usage(argv0);
  }
  return (uint64_t)n;
}

void mat_init(mat* m)
{
  mpz_init(m->a);
  mpz_init(m->b);
  mpz_init(m->c);
  mpz_init(m->d);
}

void mat_clear(mat* m)
{
  mpz_clear(m->a);
  mpz_clear(m->b);
  mpz_clear(m->c);
  mpz_clear(m->d);
}

void mat_mul(mat* out, mpz_t tmp, mat const* a, mat const* b)
{
  mpz_mul(out->a, a->a, b->a);
  mpz_mul(tmp, a->b, b->c);
  mpz_add(out->a, out->a, tmp);

  mpz_mul(out->b, a->a, b->b);
  mpz_mul(tmp, a->b, b->d);
  mpz_add(out->b, out->b, tmp);

  mpz_mul(out->c, a->c, b->a);
  mpz_mul(tmp, a->d, b->c);
  mpz_add(out->c, out->c, tmp);

  mpz_mul(out->d, a->c, b->b);
  mpz_mul(tmp, a->d, b->d);
  mpz_add(out->d, out->d, tmp);
}

void mat_swap(mat* a, mat* b)
{
  mat tmp;

  memcpy(&tmp, a, sizeof(mat));
  memmove(a, b, sizeof(mat));
  memcpy(b, &tmp, sizeof(mat));
}

void mat_pow(mat* out, mat* tmp, mpz_t tmp2, mat* in, uint64_t n)
{
  mpz_set_ui(out->a, 1);
  mpz_set_ui(out->b, 0);
  mpz_set_ui(out->c, 0);
  mpz_set_ui(out->d, 1);
  if (n == 0) {
    return;
  }
  while (n > 1) {
    if (n & 1) {
      mat_swap(tmp, out);
      mat_mul(out, tmp2, in, tmp);
    }
    mat_mul(tmp, tmp2, in, in);
    mat_swap(tmp, in);
    n >>= 1;
  }
  mat_swap(tmp, out);
  mat_mul(out, tmp2, in, tmp);
}

void fib(mpz_t ret, uint64_t n)
{
  mat out, tmp, in;
  mpz_t tmp2;

  mat_init(&out);
  mat_init(&tmp);
  mpz_init_set_ui(in.a, 1);
  mpz_init_set_ui(in.b, 1);
  mpz_init_set_ui(in.c, 1);
  mpz_init_set_ui(in.d, 0);
  mpz_init(tmp2);

  mat_pow(&out, &tmp, tmp2, &in, n);
  mpz_set(ret, out.a);

  mat_clear(&out);
  mat_clear(&tmp);
  mat_clear(&in);
  mpz_clear(tmp2);
}

int main(int argc, char const* argv[])
{
  uint64_t  n;
  mpz_t     res;

  if (argc != 2) {
    usage(argv[0]);
  }
  n = read_n(argv[0], argv[1]);
  mpz_init(res);
  fib(res, n);
  mpz_out_str(stdout, 10, res);
  putchar('\n');
  mpz_clear(res);
  return 0;
}
