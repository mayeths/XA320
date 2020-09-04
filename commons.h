#pragma once

#include <stdio.h>
#include <stdlib.h>

// -------------------- UNROLL ---------------------- //
// DO NOT use unroll without braces.
#define X5(inst) inst inst inst inst inst
#define X10(inst) X5(inst) X5(inst)
#define X50(inst) X10(inst) X10(inst) X10(inst) X10(inst) X10(inst)
#define X100(inst) X50(inst) X50(inst)
#define X500(inst) X100(inst) X100(inst) X100(inst) X100(inst) X100(inst)
#define X1000(inst) X500(inst) X500(inst)

// -------------------- COUNTER ---------------------- //

#define GET_SYS_REG(v, REG_NAME) \
  ({                             \
    asm volatile(                \
        "isb;"                   \
        "mrs %0, " REG_NAME ";"  \
        : "=r"(v));              \
  })
#define ARM_COUNTER(v) GET_SYS_REG(v, "CNTVCT_EL0")
#define ARM_COUTNER_FREQUENCY(v) GET_SYS_REG(v, "CNTFRQ_EL0")
#define TICK(v) ARM_COUNTER(v);
#define TOCK(v) { u_int64_t t_; ARM_COUNTER(t_); v = t_ - v; }

u_int64_t counter_ns() {
  u_int64_t t;
  ARM_COUTNER_FREQUENCY(t);
  return 1e9 / t;
}

void display_counter_info() {
  u_int32_t ns = counter_ns();
  u_int32_t freq = 1e9 / ns;
  u_int32_t mhz = freq / 1e6;
  printf(
    "* System counter info (read from CNTFRQ_EL0): "
    "%uMHz, %uns, %uticks\n", mhz, ns, freq);
}

// -------------------- CPU CLOCK ---------------------- //
// We assume each ADD instruction take 1 cycle
// because the following result is about
// 0.385ns(2.6GHz) on Huawei Kunpeng 920.
double cpu_clock() {
  const int repeats = 1000000;
  register int a, b;
  register u_int64_t counter, i;
  TICK(counter);
  for (i = 0; i < repeats; i += 1000) {
    X1000(asm volatile("add %0, %0, %1;" : "=r"(a) : "r"(b));)
  }
  TOCK(counter);
  u_int64_t total_ns = counter * counter_ns();
  double ns_per_inst = (double)total_ns / repeats;
  return ns_per_inst;
}
