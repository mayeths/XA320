#include "counter.h"
#include <stdlib.h>
#include <stdio.h>

void a() {
  int i[1024];
  unsigned int u[1024];
  float s[1024];
  double d[1024];
  // TEST("sdiv", );
  // TEST("smul", );
}

int main(void) {
  int a = 1, b = 2;
  unsigned long t;
  TIK(t);
#define INST_1 "sdiv %0, %0, %1;"
#define INST_10 INST_1 INST_1 INST_1 INST_1 INST_1 INST_1 INST_1 INST_1 INST_1 INST_1
#define INST_100 INST_10 INST_10 INST_10 INST_10 INST_10 INST_10 INST_10 INST_10 INST_10 INST_10
#define INST_1000 INST_100 INST_100 INST_100 INST_100 INST_100 INST_100 INST_100 INST_100 INST_100 INST_100
  asm volatile(
    INST_1000
    :"=r"(a)
    :"r"(b)
  );
  TOK(t);
  printf("a = %d, b = %d\n", a, b);
  printf("%ld\n", t);

  return 0;
}
