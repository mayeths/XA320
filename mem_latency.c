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
u_int64_t pointer_chasing(ptr_t *arr, int numb, int jump, int* repeats);

int main(int argc, char **argv) {
  int sizes[] = {
    KB(32), KB(64), KB(128), KB(256), KB(512),
    MB(1), MB(2), MB(4), MB(8), MB(16), MB(32),
    MB(48), MB(64), MB(96)
  };
  int strides[] = {
    16, 32, 64, 128, 256, 512,
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
      int jump = stride / ELEMENT_SIZE - 1;
      int repeats = 0;
      ptr_t *arr = malloc(size);
      u_int64_t counter = pointer_chasing(arr, numb, jump, &repeats);
      u_int64_t total_ns = counter * counter_ns();
      printf("%10lu %10lu %8.4f\n",
        size, stride, (double)total_ns / (double)repeats);
      free(arr);
    }
    printf("\n");
  }

  return 0;
}

u_int64_t pointer_chasing(ptr_t *arr, int numb, int jump, int* repeats) {
  memset(arr, 0, numb * ELEMENT_SIZE);
  *repeats = 0;
  register int i;
  for (i = 0; arr[i] == 0; i = (i + jump) % numb) {
    arr[i] = arr + ((i + jump) % numb);
    *repeats += 1;
  }

  register ptr_t *ptr = arr[0];
  do { ptr = *ptr; } while (ptr != arr);

  register u_int64_t counter;
  *ptr = arr[0];
  TICK(counter);
  do { ptr = *ptr; } while (ptr != arr);
  TOCK(counter);

  return counter;
}
