#include <stdlib.h>

#define ALLOC_ERROR 10
// stretchy buffer // init: NULL // free: sbfree() // push_back: sbpush() // size: sbsize() // no of elements: sbcount() // a- vector, v- value
#define sbfree(a)         ((a) ? free(stb__sbraw(a)),0 : 0, (a)=0)
#define sbpush(a,v)       (stb__sbmaybegrow(a,1), (a)[stb__sbn(a)++] = (v))
#define sbsize(a)         ((a) ? stb__sbm(a) : 0)
#define sbcount(a)        ((a) ? stb__sbn(a) : 0)
#define sbcount_inc(a)	  ((a) ? stb__sbn(a)++ : 0);		//use it if you define array vals randomly

#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */

#define sbresize(a,n)	  (!(a) ? stb__sbmaybegrow(a,n) : sbadd(a,-(sbsize(a)-n)), \
							sbsize(a) < sbcount(a) ? stb__sbn(a)-=(sbcount(a)-sbsize(a)) : 0)

#define sbadd(a,n)        (stb__sbmaybegrow(a,n), stb__sbm(a)+=(n))//, &(a)[stb__sbm(a)-(n)])	//to add/sub (+/-) more places in the vector
#define sblast(a)         ((a)[stb__sbn(a)-1])												//returns the last mem block fm the vector irrespective of it's availibility
#define stb__sbraw(a) ((int *) (a) - 2)														//the vector returned to the pointer
#define stb__sbm(a)   stb__sbraw(a)[0]														//The number of elements in the buffer including the empty ones as well
#define stb__sbn(a)   stb__sbraw(a)[1]														//The number of elements in the buffer- same as size

#define stb__sbneedgrow(a,n)  ((a)==0 || stb__sbn(a)+n >= stb__sbm(a))
#define stb__sbmaybegrow(a,n) (stb__sbneedgrow(a,(n)) ? stb__sbgrow(a,n) : 0)
#define stb__sbgrow(a,n)  stb__sbgrowf((void **) &(a), (n), sizeof(*(a)))

void stb__sbgrowf(void **arr, int increment, int itemsize);