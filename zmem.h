#pragma once

// TODOS:
// - bounds checking
// - curroption checking
// - diagnostics/memory dump
// - realloc?

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

static uint32 num_of_allocs;
static uint32 num_of_frees;
static uint32 num_of_blocks;
static uint32 num_of_cached_blocks;
static uint32 num_of_static_blocks;

void ZMEM_DEBUG_Count_Static_Blocks();
