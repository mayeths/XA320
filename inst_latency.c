// Compile: gcc -o inst_latency inst_latency.c
// Usage: ./inst_latency
#include <stdio.h>
#include <stdlib.h>
#include "commons.h"

#define REPEATS 1000000

#define TEST_CORE(op, inst)                               \
  {                                                       \
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

#define TEST_FCORE(op, inst)                              \
  {                                                       \
    asm volatile("fmov d0, %0;" :: "r"(da));              \
    asm volatile("fmov d1, %0;" :: "r"(db));              \
    asm volatile("fmov d2, %0;" :: "r"(dc));              \
    TICK(counter);                                        \
    for (i = 0; i < REPEATS; i += 1000) {                 \
      X1000(asm volatile(op" d0, d0, d1"););              \
    }                                                     \
    TOCK(counter);                                        \
    u_int64_t total_ns = counter * counter_ns();          \
    double ns = (double)total_ns / REPEATS;               \
    double cpi = ns / cpu_clock();                        \
    u_int64_t latency = (int)(cpi + 0.5);                 \
    printf("%5s %ld cycles(%.2f ns)\n", op, latency, ns); \
  }

#define TEST_2(op) TEST_CORE(op, asm volatile(op " %0, %0, %1;" : "=r"(a) : "r"(b));)
#define TEST_3(op) TEST_CORE(op, asm volatile(op " %0, %0, %1, %2;" : "=r"(a) : "r"(b), "r"(c));)
#define TEST_F2(op) TEST_FCORE(op, asm volatile(op" d0, d0, d1");)
#define TEST_F3(op) TEST_FCORE(op, asm volatile(op" d0, d0, d1, d2");)

int main() {
  display_counter_info();
  register int a, b, c;
  register double da, db, dc;
  register u_int64_t counter, i;
  TEST_2("ADD");
  TEST_2("SUB");
  TEST_2("MUL");
  TEST_2("SDIV");
  TEST_2("AND");
  TEST_2("EOR");
  TEST_2("ORR");
  TEST_2("ASR");
  TEST_2("LSL");
  TEST_2("LSR");
  TEST_2("ROR");
  TEST_3("MADD");
  TEST_F2("FADD");
  TEST_F2("FSUB");
  TEST_F2("FMUL");
  TEST_F2("FDIV");
  return 0;
}
