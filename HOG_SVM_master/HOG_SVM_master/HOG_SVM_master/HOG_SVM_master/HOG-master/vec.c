#include <stdio.h>
#include "vec.h"

#define vecassert(ptr)							\
{												\
	if(!ptr)									\
	{											\
		free(ptr);								\
		exit(ALLOC_ERROR);						\
	}											\
}
	
void stb__sbgrowf(void **arr, int increment, int itemsize)
{
    int m = *arr ? stb__sbm(*arr)+increment : increment+1;
    void *p = realloc(*arr ? stb__sbraw(*arr) : 0, itemsize * m + sizeof(int)*2);
    vecassert(p);    
    if (!*arr) ((int *) p)[1] = 0;
    *arr = (void *) ((int *) p + 2);
    stb__sbm(*arr) = m;
}
