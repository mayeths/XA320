#include <stdio.h>
#include <stdlib.h>

#define GET_SYS_REG(v, REG_NAME) \
  ({                             \
    asm volatile(                \
        "isb;"                   \
        "mrs %0, " REG_NAME ";"  \
        : "=r"(v));              \
  })
#define ARM_COUNTER(v) GET_SYS_REG(v, "CNTVCT_EL0")
#define ARM_COUTNER_FREQUENCY(v) GET_SYS_REG(v, "CNTFRQ_EL0")
#define TIK(v) ARM_COUNTER(v);
#define TOK(v) { u_int64_t t_; ARM_COUNTER(t_); v = t_ - v; }

u_int64_t counter() {
  u_int64_t t;
  ARM_COUNTER(t);
  return t;
}

u_int32_t counter_freq() {
  u_int32_t t;
  ARM_COUTNER_FREQUENCY(t);
  return t;
}

void display_counter_info() {
  u_int32_t freq = counter_freq();
  u_int32_t mhz = freq / 1e6;
  u_int32_t ns = 1e9 / freq;
  printf("* System counter info (read from CNTFRQ_EL0): ""%utick, %uMHz, %uns\n",
      freq, mhz, ns);
}

// We think each ADD instruction take 1 cycle
// because the following result is about 0.385ns(2.6GHz)
// if we run on Huawei Kunpeng 920.
double cpu_clock() {
  register int a, b;
  register u_int64_t counter, i;
#define TIMES 1000000
#define I1 "add %0, %0, %1;"
#define I5 I1 I1 I1 I1 I1
#define I10 I5 I5
#define I50 I10 I10 I10 I10 I10
#define I100 I50 I50
#define I500 I100 I100 I100 I100 I100
#define I1000 I500 I500
  TIK(counter);
  for (i = 0; i < TIMES; i += 1000) asm volatile(I1000 : "=r"(a) : "r"(b));
  TOK(counter);
  u_int64_t ns_per_tick = 1e9 / counter_freq();
  u_int64_t total_ns = counter * ns_per_tick;
  double ns_per_inst = (double)total_ns / TIMES;
  return ns_per_inst;
}
