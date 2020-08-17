// Compile: gcc -o inst_latency inst_latency.c
// Usage: ./inst_latency
#include <stdio.h>
#include <stdlib.h>
#include "commons.h"

#define REPEATS 1000000

#define TEST_CORE(op, core_inst, setup_inst)              \
  {                                                       \
    setup_inst                                            \
    TICK(counter);                                        \
    for (i = 0; i < REPEATS; i += 1000) {                 \
      X1000(core_inst);                                   \
    }                                                     \
    TOCK(counter);                                        \
    u_int64_t total_ns = counter * counter_ns();          \
    double ns = (double)total_ns / REPEATS;               \
    double cpi = ns / cpu_clock();                        \
    u_int64_t latency = (int)(cpi + 0.5);                 \
    printf("%5s %ld cycles(%.2f ns)\n", op, latency, ns); \
  }

#define TEST_I2(op) TEST_CORE(                            \
  op, asm volatile(op" %0, %0, %1;" : "=r"(a) : "r"(b));, \
  )
#define TEST_I3(op) TEST_CORE(                                        \
  op, asm volatile(op" %0, %0, %1, %2;" : "=r"(a) : "r"(b), "r"(c));, \
  )
#define TEST_F2(op) TEST_CORE(             \
  op, asm volatile(op" d0, d0, d1");,      \
  asm volatile("fmov d0, %0;" :: "r"(da)); \
  asm volatile("fmov d1, %0;" :: "r"(db)); \
  )
#define TEST_F3(op) TEST_CORE(             \
  op, asm volatile(op" d0, d0, d1, d2");,  \
  asm volatile("fmov d0, %0;" :: "r"(da)); \
  asm volatile("fmov d1, %0;" :: "r"(db)); \
  asm volatile("fmov d2, %0;" :: "r"(dc)); \
  )

int main() {
  display_counter_info();
  register int a, b, c;
  register double da, db, dc;
  register u_int64_t counter, i;
  TEST_I2("ADD");
  TEST_I2("SUB");
  TEST_I2("MUL");
  TEST_I2("SDIV");
  TEST_I2("AND");
  TEST_I2("EOR");
  TEST_I2("ORR");
  TEST_I2("ASR");
  TEST_I2("LSL");
  TEST_I2("LSR");
  TEST_I2("ROR");
  TEST_I3("MADD");
  TEST_F2("FADD");
  TEST_F2("FSUB");
  TEST_F2("FMUL");
  TEST_F2("FDIV");
  return 0;
}
