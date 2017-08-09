/* Summer 2017 */
#ifndef SETINCACHE_H
#define SETINCACHE_H


/*
	Takes in a cache and block number and value (either 1 or 0) and sets
	the valid bit at that block number to the value given.
*/
void setValid(cache_t* cache, uint32_t blockNumber, uint8_t value);

/*
	Takes in a cache and block number and value (either 1 or 0) and sets
	the dirty bit at that block number to the value given.
*/
void setDirty(cache_t* cache, uint32_t blockNumber, uint8_t value);

/*
	Takes in a cache and block number and value (either 1 or 0) and sets
	the shared bit at that block number to the value given.
*/
void setShared(cache_t* cache, uint32_t blockNumber, uint8_t value);

/*
	Takes in a cache, a location, and a value (either 0 or 1) and sets the bit
	at that bit location in the cache to the value passed in.
*/
void setBit(cache_t* cache, uint64_t location, uint8_t value);

/*
	Takes in a cache, a block number, and an LRU value and sets the LRU for
	that block number to the LRU value passed in.
*/
void setLRU(cache_t* cache, uint32_t blockNumber, long newLRU);

/* 
	Takes in a cache, a pointer to data, a block number, a length or the data,
	and an offset value. Sets the data in the block given by the block number
	at the offset specified.
*/
void setData(cache_t* cache, uint8_t* data, uint32_t blockNumber, uint32_t length, uint32_t offset);

/*
	Takes in a cache, tag, and block numbers sets the tag for the block 
	specified to be the value passed in.
*/
void setTag(cache_t* cache, uint32_t tag, uint32_t blockNumber);

/*
	Takes a newly initialized cache or a cache which has shifted programs and
	sets all of the valid bits to 0. Effectively clears the cache.
*/
void clearCache(cache_t* cache);

/*
	Takes in a cache that is switching between programs and clears it, writing
	an dirty values to memory.
*/
void contextSwitch(cache_t* cache);

/*
	Takes in a newly created cache or cleared cache and initialises all LRU 
	values to be maximal.
*/
void initializeLRU(cache_t* cache);

/*
	Takes a in a cache, the tag, and index of the accessed data
	along with the original LRU way and updates all of the LRU's in the
	cache that need to be updated.
*/
void updateLRU(cache_t* cache, uint32_t tag, uint32_t idx, long old_LRU);

#endif