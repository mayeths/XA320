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
#define TOK(v) {u_int64_t t_; ARM_COUNTER(t_); v = t_ - v; }

void display_counter_info() {
  u_int32_t freq;
  ARM_COUTNER_FREQUENCY(freq);
  u_int32_t mhz = freq / 1e6;
  u_int32_t ns = 1e9 / freq;
  u_int64_t curr;
  ARM_COUNTER(curr);
  printf("* System counter info (read from CNTFRQ_EL0): %utick, %uMHz, %uns\n",
      freq, mhz, ns);
  printf("* Current counter value (read from CNTVCT_EL0): %ld\n", curr);
}
