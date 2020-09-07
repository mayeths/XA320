// Compile: gcc -o mem_latency mem_latency.c
// Usage: ./mem_latency
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons.h"

typedef void* ptr_t;
#define ELEMENT_SIZE sizeof(ptr_t)
#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))
#define KB(n) (1024 * (n))
#define MB(n) (1024 * 1024 * (n))
#define REPEATS 24000000
u_int64_t pointer_chasing(ptr_t *arr, int numb, int jump);

int main(int argc, char **argv) {
  int sizes[] = {
    KB(32), KB(64), KB(128), KB(256), KB(512),
    MB(1), MB(2), MB(4), MB(8), MB(16), MB(32),
    MB(48), MB(64), MB(96)
  };
  int strides[] = {
    8, 16, 32, 64, 128, 256, 512,
    KB(1), KB(2), KB(4), KB(8), KB(16), KB(32),
    KB(64), KB(128), KB(256), KB(512),
    MB(1), MB(2), MB(4)
  };

  int i, j;
  for (i = 0; i < LEN(sizes); i++) {
    for (j = 0; j < LEN(strides); j++) {
      int size = sizes[i];
      int stride = strides[j];
      int numb = size / ELEMENT_SIZE;
      int jump = stride / ELEMENT_SIZE;
      ptr_t *arr = malloc(size);
      u_int64_t counter = pointer_chasing(arr, numb, jump);
      u_int64_t total_ns = counter * counter_ns();
      printf(
        "%10lu %10lu %5.2fns\n",
        size, stride, (double)total_ns / REPEATS);
      free(arr);
    }
    printf("\n");
  }

  return 0;
}

// ------------------------------------------ //
// Pointer chasing. Require numb % jump == 0
// Backward chasing. e.g,
// arr[4] = arr + 0, arr[8] = arr + 4, arr[12] = arr + 8
u_int64_t pointer_chasing(ptr_t *arr, int numb, int jump) {
  memset(arr, 0, numb * ELEMENT_SIZE);
  register int i;
  for (i = jump; i < numb; i += jump) {
    arr[i] = arr + (i - jump);
  }
  arr[0] = arr + (i - jump);  // assert arr + (i - jump) point to last element

  register ptr_t *ptr = arr + (i - jump);
  while (ptr > arr) ptr = *ptr;  // warm-up from right to left

  register u_int64_t counter;
  TICK(counter);
  for (i = 0; i < REPEATS; i+= 1000) {
    X1000(ptr = *ptr;)
  }
  TOCK(counter);

  return counter;
}
