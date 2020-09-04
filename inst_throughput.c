// Compile: gcc -o inst_throughput inst_throughput.c
// Usage: ./inst_throughput
#include <stdio.h>
#include <stdlib.h>
#include "commons.h"

#define REPEATS 1000000

#define TEST_CORE(op, core_inst)                        \
  {                                                     \
    TICK(counter);                                      \
    for (i = 0; i < REPEATS; i += 1000 * 10) {          \
      X1000(core_inst);                                 \
    }                                                   \
    TOCK(counter);                                      \
    u_int64_t total_ns = counter * counter_ns();        \
    double gips = (double)REPEATS / (double)total_ns;   \
    printf(                                             \
      "%5s %.2f GIPS (Giga inst per sec)\n", op, gips); \
  }

#define TEST_ICORE(op, m, n) TEST_CORE(op,            \
  asm volatile(                                       \
    op" %0,%10"m"%10"n"%10;\n"                        \
    op" %1,%10"m"%10"n"%10;\n"                        \
    op" %2,%10"m"%10"n"%10;\n"                        \
    op" %3,%10"m"%10"n"%10;\n"                        \
    op" %4,%10"m"%10"n"%10;\n"                        \
    op" %5,%10"m"%10"n"%10;\n"                        \
    op" %6,%10"m"%10"n"%10;\n"                        \
    op" %7,%10"m"%10"n"%10;\n"                        \
    op" %8,%10"m"%10"n"%10;\n"                        \
    op" %9,%10"m"%10"n"%10;\n"                        \
    : "=r"(a[0]), "=r"(a[1]), "=r"(a[2]), "=r"(a[3]), \
      "=r"(a[4]), "=r"(a[5]), "=r"(a[6]), "=r"(a[7]), \
      "=r"(a[8]), "=r"(a[9])                          \
    :  "r"(a[10])                                     \
);)

#define TEST_FCORE(op, m, n) TEST_CORE(op, \
  asm volatile(                            \
    op" s0,s10"m"s10"n"s10;\n"             \
    op" s1,s10"m"s10"n"s10;\n"             \
    op" s2,s10"m"s10"n"s10;\n"             \
    op" s3,s10"m"s10"n"s10;\n"             \
    op" s4,s10"m"s10"n"s10;\n"             \
    op" s5,s10"m"s10"n"s10;\n"             \
    op" s6,s10"m"s10"n"s10;\n"             \
    op" s7,s10"m"s10"n"s10;\n"             \
    op" s8,s10"m"s10"n"s10;\n"             \
    op" s9,s10"m"s10"n"s10;\n"             \
);)

#define TEST_DCORE(op, m, n) TEST_CORE(op, \
  asm volatile(                            \
    op" d0,d10"m"d10"n"d10;\n"             \
    op" d1,d10"m"d10"n"d10;\n"             \
    op" d2,d10"m"d10"n"d10;\n"             \
    op" d3,d10"m"d10"n"d10;\n"             \
    op" d4,d10"m"d10"n"d10;\n"             \
    op" d5,d10"m"d10"n"d10;\n"             \
    op" d6,d10"m"d10"n"d10;\n"             \
    op" d7,d10"m"d10"n"d10;\n"             \
    op" d8,d10"m"d10"n"d10;\n"             \
    op" d9,d10"m"d10"n"d10;\n"             \
);)

#define TEST_VCORE(op, m, n) TEST_CORE(op, \
  asm volatile(                            \
    op" v0.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v1.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v2.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v3.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v4.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v5.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v6.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v7.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v8.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
    op" v9.4s,v10.4s"m"v10.4s"n"v10.4s;\n" \
);)

// ------------------------------------------ //
// I: accept 32bits and 64bits integer instruction
// F: accept 32bits floating point instruction
// D: accept 64bits floating point instruction
// V: accept 32bitsx4 integer/floating point instruction
#define TEST_I1(op) TEST_ICORE(op, "//", "  ")
#define TEST_F1(op) TEST_FCORE(op, "//", "  ")
#define TEST_D1(op) TEST_DCORE(op, "//", "  ")
#define TEST_V1(op) TEST_VCORE(op, "//", "  ")
#define TEST_I2(op) TEST_ICORE(op, ", ", "//")
#define TEST_F2(op) TEST_FCORE(op, ", ", "//")
#define TEST_D2(op) TEST_DCORE(op, ", ", "//")
#define TEST_V2(op) TEST_VCORE(op, ", ", "//")
#define TEST_I3(op) TEST_ICORE(op, ", ", ", ")
#define TEST_F3(op) TEST_FCORE(op, ", ", ", ")
#define TEST_D3(op) TEST_DCORE(op, ", ", ", ")
#define TEST_V3(op) TEST_VCORE(op, ", ", ", ")

// ------------------------------------------ //
// We need -O3 to make sure unrolling in TEST_CORE works.
__attribute__ ((optimize(3)))
int main() {
  register int a[11];
  register u_int64_t counter, i;
  double cpu_ghz = 1 / cpu_clock();
  printf("CPU runs at %.2f GHz\n", cpu_ghz);
  TEST_I1("MOV");
  TEST_I2("MUL");
  TEST_I3("MADD");
  TEST_F2("FADD");
  TEST_V2("ADD");
}
