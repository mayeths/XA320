#include <stdio.h>
#include <stdlib.h>
#include "counter.h"

#define TIMES 1000000
#define x1(op) op " %0, %0, %1;"
#define x5(op) x1(op) x1(op) x1(op) x1(op) x1(op)
#define x10(op) x5(op) x5(op)
#define x50(op) x10(op) x10(op) x10(op) x10(op) x10(op)
#define x100(op) x50(op) x50(op)
#define x500(op) x100(op) x100(op) x100(op) x100(op) x100(op)
#define x1000(op) x500(op) x500(op)
#define TEST_INST(op)                                                \
  {                                                                  \
    register int a_, b_;                                             \
    register u_int64_t counter_, i_;                                 \
    TIK(counter_);                                                   \
    for (i_ = 0; i_ < TIMES; i_ += 1000) {                           \
      asm volatile(x1000(op) : "=r"(a_) : "r"(b_));                  \
    }                                                                \
    TOK(counter_);                                                   \
    u_int64_t ns_per_tick_ = 1e9 / counter_freq();                   \
    u_int64_t total_ns_ = counter_ * ns_per_tick_;                   \
    double ns_per_inst_ = (double)total_ns_ / TIMES;                 \
    double cpi_ = ns_per_inst_ / cpu_clock();                        \
    u_int64_t latency_ = (int)(cpi_ + 0.5);                          \
    printf("%5s %ld cycles(%.2f ns)\n", op, latency_, ns_per_inst_); \
  }

int main(void) {
  display_counter_info();
  TEST_INST("ADD");
  TEST_INST("SUB");
  TEST_INST("MUL");
  TEST_INST("SDIV");
  return 0;
}
