#include <stdlib.h>
#include <stdio.h>
#include "counter.h"
#define ITER_SIZE 100000000

#define OP(op1, op2)             \
  x[0] = x[0] op1 y[0] op2 z[0]; \
  x[1] = x[1] op1 y[0] op2 z[0]; \
  x[2] = x[2] op1 y[0] op2 z[0]; \
  x[3] = x[3] op1 y[0] op2 z[0]; \
  x[4] = x[4] op1 y[0] op2 z[0]; \
  x[5] = x[5] op1 y[0] op2 z[0]; \
  x[6] = x[6] op1 y[0] op2 z[0]; \
  x[7] = x[7] op1 y[0] op2 z[0];

#define OMP_PRAGMA() \
 _Pragma("omp parallel for private(iter, idx, x, y, z) num_threads(nthreads)")

#define TEST_OP3(name, test_t, op1, op2) {              \
  test_t x[8], y[8], z[8];                              \
  u_int64_t counter, iter, idx;                         \
  for (idx = 0; idx < 8; idx++) {                       \
    x[idx] = 1.0;                                       \
    y[idx] = 1.00001;                                   \
  }                                                     \
  TIK(counter)                                          \
  OMP_PRAGMA()                                        /*\
                a trick to output newline in macro    */\
  for (iter = 0; iter < ITER_SIZE; iter++) OP(op1, op2) \
  TOK(counter)                                          \
  printf("%4s: %10ld\n", name, counter);                \
  }

#define TEST_OP2(name, test_t, op1) TEST_OP3(name, test_t, op1, ;)

int main(int argc, char **argv) {
  int nthreads = 10;
  if (argc > 1) nthreads = atoi(argv[1]);
  TEST_OP2("addd", double, +);
  TEST_OP2("subd", double, -);
  TEST_OP2("muld", double, *);
  TEST_OP2("divd", double, /);
  TEST_OP2("adds", float, +);
  TEST_OP2("subs", float, -);
  TEST_OP2("muls", float, *);
  TEST_OP2("divs", float, /);
  TEST_OP3("mad", double, *, +);
  TEST_OP3("mas", float, *, +);
}
