#include <stdlib.h>
#include <omp.h>

int main() {
  register unsigned long timer;
  double *A = malloc(sizeof(double) * 1024);
  double *B = malloc(sizeof(double) * 1024);
  double *C = malloc(sizeof(double) * 1024);

#define INST_1 ptr = *ptr;
#define INST_5 INST_1 INST_1 INST_1 INST_1 INST_1
#define INST_10 INST_5 INST_5
#define INST_50 INST_10 INST_10 INST_10 INST_10 INST_10
#define INST_100 INST_50 INST_50
#define INST_500 INST_100 INST_100 INST_100 INST_100 INST_100
#define INST_1000 INST_500 INST_500
#pragma omp parallel for
  for (int i = 0; i < 100000000; i++) {

  }

// 执行10000000个指令，看耗时


}
