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

static char const* argv0;

[[noreturn]] static void
usage(void)
{
  fprintf(stderr, "usage: %s [base] <n>\nfib 0.2 - gmp/%s\n", argv0,
          gmp_version);
  exit(1);
}

static uint64_t
read_n(char const* arg)
{
  _Static_assert(sizeof(unsigned long long) <= sizeof(uint64_t));
  unsigned long long n;
  char *end;

  while (isspace(*arg)) {
    ++arg;
  }
  if (*arg == '-') {
    usage();
  }
  errno = 0;
  n = strtoull(arg, &end, 0);
  if ((n == 0 && end == arg) ||
      (n == ULLONG_MAX && errno) ||
      *end) {
    usage();
  }
  return (uint64_t)n;
}

static void
mat_init(mat* m)
{
  mpz_init(m->a);
  mpz_init(m->b);
  mpz_init(m->c);
  mpz_init(m->d);
}

static void
mat_clear(mat* m)
{
  mpz_clear(m->a);
  mpz_clear(m->b);
  mpz_clear(m->c);
  mpz_clear(m->d);
}

static void
mat_swap(mat* restrict a, mat* restrict b)
{
  mat tmp;

  memcpy(&tmp, a, sizeof(mat));
  memcpy(a, b, sizeof(mat));
  memcpy(b, &tmp, sizeof(mat));
}

static void
mat_mul(mat* restrict out, mpz_ptr restrict tmp, mat const* a, mat const* b)
{
  mpz_mul(tmp, a->a, b->a);
  mpz_mul(out->a, a->b, b->c);
  mpz_add(out->a, out->a, tmp);

  mpz_mul(tmp, a->a, b->b);
  mpz_mul(out->b, a->b, b->d);
  mpz_add(out->b, out->b, tmp);

  mpz_mul(tmp, a->c, b->a);
  mpz_mul(out->c, a->d, b->c);
  mpz_add(out->c, out->c, tmp);

  mpz_mul(tmp, a->c, b->b);
  mpz_mul(out->d, a->d, b->d);
  mpz_add(out->d, out->d, tmp);
}

static void
mat_pow(mat* restrict out, mat* restrict tmp, mpz_ptr restrict tmp2,
        mat* restrict in, uint64_t n)
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
      mat_mul(tmp, tmp2, in, out);
      mat_swap(out, tmp);
    }
    mat_mul(tmp, tmp2, in, in);
    mat_swap(in, tmp);
    n >>= 1;
  }
  mat_mul(tmp, tmp2, in, out);
  mat_swap(out, tmp);
}

static void
fib(mpz_t ret, uint64_t n)
{
  mat out, tmp, in;

  if (n == 0) {
    mpz_set_ui(ret, 0);
    return;
  }
  mat_init(&out);
  mat_init(&tmp);
  mpz_init_set_ui(in.a, 1);
  mpz_init_set_ui(in.b, 1);
  mpz_init_set_ui(in.c, 1);
  mpz_init_set_ui(in.d, 0);

  mat_pow(&out, &tmp, ret, &in, n - 1);
  mpz_swap(ret, out.a);

  mat_clear(&out);
  mat_clear(&tmp);
  mat_clear(&in);
}

int
main(int argc, char const* const argv[])
{
  uint64_t  n;
  int       base;
  mpz_t     res;

  argv0 = argv[0];
  switch (argc) {
    case 2:
      base = 10;
      n = read_n(argv[1]);
      break;
    case 3:
      base = atoi(argv[1]);
      if (!(base <= -2 && base >= -36) && !(base >= 2 && base <= 62)) {
        fprintf(stderr, "base must be in -2..-36 or 2..62\n");
        usage();
      }
      n = read_n(argv[2]);
      break;
    default:
      usage();
  }
  mpz_init(res);
  fib(res, n);
  mpz_out_str(stdout, base, res);
  putchar('\n');
  mpz_clear(res);
  return 0;
}
