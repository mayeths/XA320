// Compile: gcc -o inst_latency inst_latency.c
// Usage: ./inst_latency
#include <stdio.h>
#include <stdlib.h>
#include "commons.h"

#define REPEATS 1000000

#define TEST_CORE(op, inst)                               \
  {                                                       \
    register int a, b, c;                                 \
    register u_int64_t counter, i;                        \
    TICK(counter);                                        \
    for (i = 0; i < REPEATS; i += 1000) {                 \
      X1000(inst);                                        \
    }                                                     \
    TOCK(counter);                                        \
    u_int64_t total_ns = counter * counter_ns();          \
    double ns = (double)total_ns / REPEATS;               \
    double cpi = ns / cpu_clock();                        \
    u_int64_t latency = (int)(cpi + 0.5);                 \
    printf("%5s %ld cycles(%.2f ns)\n", op, latency, ns); \
  }

#define TEST_2(op) TEST_CORE(op, INST_2(op, a, b))
#define TEST_3(op) TEST_CORE(op, INST_3(op, a, b, c))

int main() {
  display_counter_info();
  TEST_2("ADD");
  TEST_2("SUB");
  TEST_2("MUL");
  TEST_2("SDIV");
  TEST_2("AND");
  TEST_2("EOR");
  TEST_2("ORR");
  TEST_2("ASR");
  //FIXME: if insert more test here like TEST_2("LSL"),
  // ADD and SUB above will become 2 cycles. Confusing.
  TEST_3("MADD");
  return 0;
}
