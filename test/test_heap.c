#include <sys/heap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void heap_debug_small(smallheap_t *heap);

uint8_t memory[256*1024];

#define MAX_ALLOC_SIZE 250

typedef struct data {
  unsigned start;
  unsigned end;
  void *ptr;
  size_t size;
} data_t;

void check_data(data_t* ptr, data_t *array, int size) {
  uintptr_t test = (uintptr_t)ptr->ptr;
  if (test % 8) {
    printf("wrong alignment!\n");
    abort();
  }
  uint32_t* test2 = (uint32_t*)ptr->ptr;
  uint32_t test3 = *(test2-1) & 0x7fffffff;
  if (test3 > MAX_ALLOC_SIZE) {
    printf("too big! %u\n", test3);
    abort();
  }
  
  for (int i = 0; i < size; ++i) {
    data_t *check = array + i;
    if (ptr == check) continue;
    if (ptr->end <= (check->start-8) || ptr->start >= (check->end+8)) {
      // ok
    } else {
      printf("overlaps %4u bytes @ %6u - %6u\n", (unsigned)check->size,
             (unsigned)check->start, (unsigned)check->end);
      abort();
    }
  }
}

int main() {
  wilderness_t wild;
  heap_init_wilderness(&wild, memory, memory + sizeof(memory), 4096);

  smallheap_t heap;
  heap_init_small(&heap, &wild);
  unsigned max_alloc = 0;
  unsigned total_alloc = 0;

  // just alloc some blocks:
  printf("allocate:\n");
  #define NOF_ALLOCATIONS 1000
  data_t data[NOF_ALLOCATIONS];
  for (int i = 0; i < NOF_ALLOCATIONS; ++i) {
    int test = rand() % MAX_ALLOC_SIZE + 1;
    data[i].size = test;
    total_alloc += data[i].size;
    if (total_alloc > max_alloc) max_alloc = total_alloc;
    data[i].ptr = heap_malloc_small(&heap, data[i].size);
    heap_debug_small(&heap);
    data[i].start = (unsigned)(((uint8_t*)data[i].ptr) - memory);
    data[i].end = data[i].start + data[i].size;
    printf("allocated %4u bytes @ %6u - %6u\n", (unsigned)data[i].size,
           (unsigned)data[i].start, (unsigned)data[i].end);
    check_data(data + i, data, i);
  }

  printf("free-allocate:\n");
  #define NOF_ITERATIONS 100000
  for (int j = 0; j < NOF_ITERATIONS; ++j) {
    int idx = rand() % NOF_ALLOCATIONS;
    total_alloc -= data[idx].size;
    if (j == 82) {
      printf("test\n");
    }
    heap_free_small(&heap, data[idx].ptr);
    heap_debug_small(&heap);
    data[idx].size = rand() % MAX_ALLOC_SIZE + 1;
    total_alloc += data[idx].size;
    if (total_alloc > max_alloc) max_alloc = total_alloc;
    data[idx].ptr = heap_malloc_small(&heap, data[idx].size);
    heap_debug_small(&heap);
    data[idx].start = (unsigned)(((uint8_t*)data[idx].ptr) - memory);
    data[idx].end = data[idx].start + data[idx].size;
    printf("allocated %4u bytes @ %6u - %6u\n", (unsigned)data[idx].size,
           (unsigned)data[idx].start, (unsigned)data[idx].end);
    check_data(data + idx, data, NOF_ALLOCATIONS);
  }

  printf("maximal allocation = %6u\n", max_alloc);
  printf("total memory used  = %6u\n", wild.bottom - wild.base);
  return 0;
}
