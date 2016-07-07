#include "zmem.h"

#include <stdlib.h>

int32 ugly_hack_last_alloc_was_cache;

void ZMEM_Init(uint32 fullsize)
{
    fullsize = ((fullsize + 4095) & ~4095); // NOTE(dShamir): page aligned
    
    zmem_root = (memory_chunk *)malloc(fullsize);

#if 1
    uint8 *p = (uint8 *)zmem_root;
    for(int i = 0; i < fullsize; i++)
    {
	*p = 0xFF;
	p++;
    }
#endif

    zmem_root->size = fullsize; // NOTE(dShamire): the root holds the entire fullsize
    zmem_root->tag = ZMEM_TAG_STATIC;
    zmem_root->free = false;

    memory_chunk *first_free = (memory_chunk *)((uint8 *)(zmem_root) + sizeof(memory_chunk));

    first_free->size = fullsize - sizeof(memory_chunk);
    first_free->tag = ZMEM_TAG_FREE;
    first_free->free = true;

    zmem_root->next = zmem_root->prev = first_free;
    first_free->next = first_free->prev = zmem_root;
}

void *ZMEM_Allocate(uint32 size, uint16 tag)
{   
    zmem_debug.num_of_allocs++;
    ugly_hack_last_alloc_was_cache = false;

    if(size == 0)
	return 0;
    
    size = ((size + 15) & ~15);
    size += sizeof(memory_chunk); // QUESTION(dShamir): should this be done before alignment?
    
    memory_chunk *block = zmem_root->prev; // this is actually the last free block
    memory_chunk *end = zmem_root->prev;

    while(!block->free || block->size < size)
    {
	block = block->next;

	if(block == end)
	{
	    return 0;
	}

	if(block->tag == ZMEM_TAG_CACHE)
	{
	    zmem_debug.num_of_cached_blocks++;
	    // TODO(dShamir): free this block and use it
	    ZMEM_Free(block + 1);
	    if(block->size >= size) // NOTE(dShamir): ZMEM_Free coalesce free blocks, even if the block is initialy too small the free could make it larger
	    {
		ugly_hack_last_alloc_was_cache = true;
		
		block->tag = tag;
		block->free = false;
		return (block + 1);
	    }
	    else
	    {
		// TODO(dShamir): do something better here
		//return 0;
	    }
	}

	if(block->free && block->size >= size)
	{
	    block->tag = tag;
	    block->free = false;
	    return (block + 1);
	}
    }

    zmem_debug.num_of_blocks++;

    memory_chunk *new_free = (memory_chunk *)((uint8 *)(block) + size);
    new_free->size = block->size - size;
    new_free->tag = ZMEM_TAG_FREE;
    new_free->free = true;

    new_free->next = block->next;
    new_free->prev = block;
    block->next = new_free;
    new_free->next->prev = new_free;

    block->size = size;
    block->tag = tag;
    block->free = false;

    return (block + 1);
}

void ZMEM_Free(void *ptr)
{
    zmem_debug.num_of_frees++;

    memory_chunk *block = (memory_chunk *)((uint8 *)(ptr) - sizeof(memory_chunk));

    if(block->free)
	return;

    if(block->next->free)
    {
	// TODO(dShamir): coalesce adjacent free blocks
    }
    if(block->prev->free)
    {
	// TODO(dShamir): coalesce adjacent free blocks
    }

    block->tag = ZMEM_TAG_FREE;
    block->free = true;

#if 1
    uint8 *p = (uint8 *)(block + 1);
    for(int i = 0; i < block->size - sizeof(memory_chunk); i++)
    {
	*p = 0xDD;
	p++;
    }
#endif
}

void ZMEM_DEBUG_Count_Static_Blocks()
{
    memory_chunk *block = zmem_root;
    memory_chunk *end = zmem_root->prev;

    while(block != end)
    {
	if(block->tag == ZMEM_TAG_STATIC)
	{
	    zmem_debug.num_of_static_blocks++;
	}

	block = block->next;
    }
}
