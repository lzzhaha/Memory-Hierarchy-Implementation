/* Summer 2017 */
#ifndef GETFROMCACHE_H
#define GETFROMCACHE_H

/*
	Takes in a cache and a blocknumber and returns that block's valid bit.
*/
uint8_t getValid(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache and a blocknumber and returns that block's dirty bit.
*/
uint8_t getDirty(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache and a blocknumber and returns that block's shared bit.
*/
uint8_t getShared(cache_t* cache, uint32_t blockNumber);

/* 
	Takes in a cache and a location of the cache in bits and returns the
	value of the bit at that location.
*/
uint8_t getBit(cache_t* cache, uint64_t location);

/*
	Takes a cache and a block number and extracts the value of the LRU 
	for the block specified.
*/
long getLRU(cache_t* cache, uint32_t blockNumber);

/*
	Takes a cache and a block number and extracts the value of the tag 
	for the block specified.
*/
uint32_t extractTag(cache_t* cache, uint32_t blockNumber);

/*
	Takes a cache and a block number and extracts the value of the index 
	for the block specified.
*/
uint32_t extractIndex(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache, a tag, a blocknumber, and an offset and extracts the
	original address.
*/
uint32_t extractAddress(cache_t* cache, uint32_t tag, uint32_t blockNumber, uint32_t offset);

/*
	Takes in a cache and an address and finds the next block that should be 
	used for a cache operation on the address provided. If this address is
	already in memory it should return the block at which this address's
	operation would occur and indicates that this was a successful match.
	If this address is not stored in the cache then it should point to the next
	block that needs to be evicted as indicated by our LRU replacement system.
	If there are multiple blocks that could be evicted selects the
	block that occurs earlier in the cache. Returns a pointer to a struct
	which contains a block number, an LRU value, and whether or not the address
	is already stored in the cache (is a match).
*/
evictionInfo_t* findEviction(cache_t* cache, uint32_t address);

/*
	Takes in a cache and an address and returns the LRU
	value of that address in the cache. Used mostly for testing.
	Returns -1 if the information is not present in the cache.
*/
long getLRUAddress(cache_t* cache, uint32_t address);

/*
	Takes in a starting location, an address, a blocknumber, and a size and
	returns a pointer to an array of the data that was read. ASSUMES THAT
	ALL THE DATA FITS IN THE BLOCK. This should be handled by a function
	higher up that calls this function.
*/
uint8_t* getData(cache_t* cache, uint32_t offset, uint32_t blockNumber, uint32_t size);
#endif