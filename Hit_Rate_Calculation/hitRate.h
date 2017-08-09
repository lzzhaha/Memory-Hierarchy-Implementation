/* Summer 2017 */
#ifndef HITRATE_H
#define HITRATE_H
#include <stdlib.h>

/*
	Function used to return the hit rate for a cache.
*/
double findHitRate(cache_t* cache);

/*
	Function used to update the cache indicating there has been a cache access.
*/
void reportAccess(cache_t* cache);

/*
	Function used to update the cache indicating there has been a cache hit.
*/
void reportHit(cache_t* cache);
#endif