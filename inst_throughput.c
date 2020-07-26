#include <stdio.h>
#include "counter.h"

#ifndef CORE_NUM
#define CORE_NUM 8
#endif
#ifndef ITER_NUM
#define ITER_NUM 1000000
#endif

int main() {
  double a[1024], b[1024], c[1024];
  int core, iter;
  u_int64_t start_clock, end_clock;
  ARM_COUNTER(start_clock);

#pragma omp parallel for private(iter, a, b, c)
  for (core = 0; core < CORE_NUM; core++) {
    for (iter = 0; iter < ITER_NUM; iter++) {
#define RAW_INST(counter) a[counter] = b[counter] + c[counter];
#define INST_1 RAW_INST(__COUNTER__)
#define INST_4 INST_1 INST_1 INST_1 INST_1
#define INST_16 INST_4 INST_4 INST_4 INST_4
#define INST_64 INST_16 INST_16 INST_16 INST_16
#define INST_256 INST_64 INST_64 INST_64 INST_64
#define INST_1024 INST_256 INST_256 INST_256 INST_256
      INST_1024;
    }
  }

  ARM_COUNTER(end_clock);
  display_counter_info();
  printf("start at %ld, end at %ld, diff %ld\n", start_clock, end_clock, end_clock - start_clock);
}
