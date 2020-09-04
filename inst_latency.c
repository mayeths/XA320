// Compile: gcc -o inst_latency inst_latency.c
// Usage: ./inst_latency
#include <stdio.h>
#include <stdlib.h>
#include "commons.h"

#define REPEATS 1000000

#define TEST_CORE(op, core_inst)                       \
  {                                                    \
    TICK(counter);                                     \
    for (i = 0; i < REPEATS; i += 1000) {              \
      X1000(core_inst);                                \
    }                                                  \
    TOCK(counter);                                     \
    u_int64_t total_ns = counter * counter_ns();       \
    double ns = (double)total_ns / REPEATS;            \
    double cpi = ns / cpu_clock();                     \
    printf("%5s %.1f cycles(%.2f ns)\n", op, cpi, ns); \
  }

// ------------------------------------------ //
// I: accept 32bits and 64bits integer instruction
// F: accept 32bits floating point instruction
// D: accept 64bits floating point instruction
// V: accept 32bitsx4 integer/floating point instruction
#define TEST_I1(op) TEST_CORE(op, asm volatile(op" %0, %0;" : "=r"(a));)
#define TEST_I2(op) TEST_CORE(op, asm volatile(op" %0, %0, %1;" : "=r"(a) : "r"(b));)
#define TEST_I3(op) TEST_CORE(op, asm volatile(op" %0, %0, %1, %2;" : "=r"(a) : "r"(b), "r"(c));)
#define TEST_F1(op) TEST_CORE(op, asm volatile(op" s0, s0");)
#define TEST_F2(op) TEST_CORE(op, asm volatile(op" s0, s0, s1");)
#define TEST_F3(op) TEST_CORE(op, asm volatile(op" s0, s0, s1, s2");)
#define TEST_D1(op) TEST_CORE(op, asm volatile(op" d0, d0");)
#define TEST_D2(op) TEST_CORE(op, asm volatile(op" d0, d0, d1");)
#define TEST_D3(op) TEST_CORE(op, asm volatile(op" d0, d0, d1, d2");)
#define TEST_V1(op) TEST_CORE(op, asm volatile(op" v0.4s, v0.4s;");)
#define TEST_V2(op) TEST_CORE(op, asm volatile(op" v0.4s, v0.4s, v1.4s;");)
#define TEST_V3(op) TEST_CORE(op, asm volatile(op" v0.4s, v0.4s, v1.4s, v2.4s;");)

int main() {
  display_counter_info();
  register int a, b, c;
  register u_int64_t counter, i;
  TEST_I1("MOV");
  TEST_I2("MUL");
  TEST_I3("MADD");
  TEST_F2("FADD");
  TEST_V2("ADD");
  return 0;
}
