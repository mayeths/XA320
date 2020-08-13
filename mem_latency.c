// Compile: gcc [-DVERBOSE] -o cache cache.c
// Usage: ./cache <size-in-KB> <stride-in-Byte>
// Example: ./cache 1024 128
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define COUNTER(t) ({asm volatile("isb;""mrs %0, CNTVCT_EL0;":"=r"(t1));})  // Macro

#define ARM_COUNTER() ({                              \
  register unsigned long val;                         \
  asm volatile("isb;""mrs %0, CNTVCT_EL0;":"=r"(val));\
  val;                                                \
})  // Return uint64 value

typedef void *ptr_t;
clock_t measure_load(ptr_t *arr, size_t len, size_t stride, size_t repeats);
void die(char *msg);

int main(int argc, char **argv) {
  if (argc <= 1) {
    die("Didn't specify the size of array (in KB) and stride");
  } else if (argc <= 2) {
    die("Didn't specify the stride.");
  } else if (atoi(argv[2]) % sizeof(ptr_t) != 0) {
    die("Stride must aligned to 8Bytes.");
  }
  size_t total_bytes = atoi(argv[1]) * 1024;
  size_t stride_bytes = atoi(argv[2]);
  size_t len = total_bytes / sizeof(ptr_t);
  size_t stride = stride_bytes / sizeof(ptr_t);
  ptr_t *arr = malloc(total_bytes);
#ifdef VERBOSE
  printf(
      "[%ld Bytes per element] "
      "[total: %ld Bytes (%ld elements)] "
      "[stride: %ld Bytes (%ld elements)]\n",
      sizeof(ptr_t), total_bytes, len, stride_bytes, stride);
#endif

  clock_t best_result = measure_load(arr, len, stride, 4);
  double ms = best_result * 1000. / CLOCKS_PER_SEC;
  printf(
      "[Size: %5d KB, Stride: %5d Bytes]"
      " best %10ld clocks (%fms)\n",
      atoi(argv[1]), atoi(argv[2]), best_result, ms);

  return 0;
}

clock_t measure_load(ptr_t *arr, size_t len, size_t stride, size_t repeats) {
  // Init
  size_t k;
  for (k = 0; k < len; k += stride) arr[k] = arr + ((k + stride) % len);
  arr[k - stride] = arr;

  ptr_t *ptr = arr;
  clock_t timer, best_result;
  for (size_t iter = 0; iter < repeats; iter++) {
#define LOAD_NUM 100000000000
#define TIK(t) t = clock();
#define TOK(t) t = clock() - t;
#define INST_1 ptr = *ptr;
#define INST_5 INST_1 INST_1 INST_1 INST_1 INST_1
#define INST_10 INST_5 INST_5
#define INST_50 INST_10 INST_10 INST_10 INST_10 INST_10
#define INST_100 INST_50 INST_50
#define INST_500 INST_100 INST_100 INST_100 INST_100 INST_100
#define INST_1000 INST_500 INST_500
    TIK(timer);
    for (size_t i = 0; i < LOAD_NUM; i += 1000) INST_1000;
    TOK(timer);
    if (timer < best_result) best_result = timer;
#ifdef VERBOSE
    printf(
        "[%ld/%ld] %ld times loading "
        "take %ld clocks\n",
        iter + 1, repeats, LOAD_NUM, timer);
#endif
  }
  return best_result;
}

void die(char *msg) {
  fprintf(stderr, "[Error] %s\n", msg);
  fprintf(stderr, "[Usage] ./executable <size-in-KB> <stride>\n");
  exit(1);
}
