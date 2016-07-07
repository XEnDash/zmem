#pragma once

// TODOS:
// - bounds checking
// - curroption checking
// - diagnostics/memory dump
// - realloc?
// - find a way to make the linked list search faster
// ---!!! for some reason allocations randomly take 0.00029325513196480938 ms for no apparaent reason (check the timing.log file) FIX THIS !!!---

#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

#define ZMEM_TAG_FREE 0
#define ZMEM_TAG_STATIC 1
#define ZMEM_TAG_CACHE 101

typedef struct memory_chunk_h
{
    uint32 size;
    uint16 tag;
    uint16 free;
    
    memory_chunk_h *prev, *next;
} memory_chunk; // 24 bytes

static memory_chunk *zmem_root;

void ZMEM_Init(uint32 fullsize);
void *ZMEM_Allocate(uint32 size, uint16 tag);
void ZMEM_Free(void *ptr);

typedef struct memory_debug_h
{
    uint32 num_of_allocs;
    uint32 num_of_frees;
    uint32 num_of_blocks;
    uint32 num_of_cached_blocks;
    uint32 num_of_static_blocks;
} memory_debug;

static memory_debug zmem_debug;

void ZMEM_DEBUG_Count_Static_Blocks();

extern int32 ugly_hack_last_alloc_was_cache;
