#include "defines.h"
#include <stdbool.h>
typedef struct MemorySegment{
    uint32_t length;
    struct MemorySegment* next;
    struct MemorySegment* previous;
    struct MemorySegment* nextfree;
    struct MemorySegment* previousfree;
    bool isfree;
}MemorySegment;
typedef struct{
    uint32_t MemorySegmentAddr : 31;
    bool IsAligned : 1;
}AlignedMemorySegment;

void InitHeap(uint32_t heapstart ,uint32_t heaplength);
void* malloc(uint32_t size);
void* aligned_alloc(uint32_t alignment,uint32_t size);
void* calloc(uint32_t size);
void* realloc(void* old,uint32_t size);
void free(void* tofree);