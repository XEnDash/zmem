#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "zmem.h"

#define MAX_POINTERS 512 * 1024

uint64 qpf_freq;
uint64 qpf_start;
uint64 qpf_end;

LARGE_INTEGER li_start;
LARGE_INTEGER li_end;

#define LARGE_INTEGER_TO_UINT64(li) ((uint64)li.HighPart << 32) | li.LowPart

#define TIMING_LOG_FILE "times.log"

int main(int argc, char **argv)
{
    srand(time(0));

    FILE *log = fopen(TIMING_LOG_FILE, "w");

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    qpf_freq = LARGE_INTEGER_TO_UINT64(freq);

    qpf_freq /= 1000; // NOTE(dShamir): frequency is in seconds, this turns it into miliseconds

    ZMEM_Init(16 * 1024 * 1024);

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

	QueryPerformanceCounter(&li_start);

	ptr[ptr_n] = (uint8 *)ZMEM_Allocate(size, tag);

	QueryPerformanceCounter(&li_end);

	qpf_start = LARGE_INTEGER_TO_UINT64(li_start);
	qpf_end = LARGE_INTEGER_TO_UINT64(li_end);

	fprintf(log, "allocation %i: %4.20f ms", ptr_n, (double)(qpf_end - qpf_start) / (double)qpf_freq);
	if(ugly_hack_last_alloc_was_cache)
	    fprintf(log, " cached");
	fprintf(log, "\n");

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

    fclose(log);

    return 0;
}
