#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "zmem.h"

#define MAX_POINTERS 512 * 1024

int main(int argc, char **argv)
{
    srand(time(0));

    ZMEM_Init(4 * 1024 * 1024);

    uint8 *ptr[MAX_POINTERS];
    uint32 ptr_n = 0;

    while(1)
    {
	uint32 size = (rand() % 1023) + 1;
	uint16 tag = ZMEM_TAG_STATIC;
	if(size % 10 > 3)
	{
	    tag = ZMEM_TAG_CACHE;
	}
	
	ptr[ptr_n] = (uint8 *)ZMEM_Allocate(size, tag);

	if(ptr[ptr_n] == 0)
	    break;

	uint8 *p = ptr[ptr_n];
	for(int i = 0; i < size; i++)
	{
	    *p = 0xAA;
	    p++;
	}

	if(size % 10 == 0)
	{
	    ZMEM_Free(ptr[ptr_n]);
	}

	ptr_n++;

	if(ptr_n >= MAX_POINTERS)
	    break;
    }

    ZMEM_DEBUG_Count_Static_Blocks();

    return 0;
}
