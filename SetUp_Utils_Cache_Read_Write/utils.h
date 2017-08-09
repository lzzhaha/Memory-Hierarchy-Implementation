/* Summer 2017 */
#ifndef UTILS_H
#define UTILS_H

/*
	Struct to be used to represent a cache. Both the block data size
	and the total data size is given in bytes. The physical Memory Name
	is the name of the file which will function as main memory for the
	cache. The access and hit fields are used to track cache accesses
	and are used for hit rate. This will be implemented in part 2 of
	the project.
*/
typedef struct cache
{
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	uint8_t* contents;
	char* physicalMemoryName;
	double access;
	double hit;
} cache_t;

/*
	A struct used to contain eviction info. It contains a block number for 
	the block that will be evicted,  a bool field indicating whether or not
	a tag was matched, and the value of the LRU of the information found.
*/
typedef struct eviction
{
	uint32_t blockNumber;
	long LRU;
	bool match;	
}evictionInfo_t;

/*
	A struct used to returns a byte cache read. It returns both
	the data present in the cache and a bool flag which displays whether
	or not the read attempt was successful.
*/
typedef struct byteInfo 
{
	bool success;
	uint8_t data;
} byteInfo_t;

/*
	A struct used to returns a halfword cache read. It returns both
	the data present in the cache and a bool flag which displays whether
	or not the read attempt was successful.
*/
typedef struct halfWordInfo 
{
	bool success;
	uint16_t data;
} halfWordInfo_t;

/*
	A struct used to returns a word cache read. It returns both
	the data present in the cache and a bool flag which displays whether
	or not the read attempt was successful.
*/
typedef struct wordInfo 
{
	bool success;
	uint32_t data;
} wordInfo_t;

/*
	A struct used to returns a double word cache read. It returns both
	the data present in the cache and a bool flag which displays whether
	or not the read attempt was successful.
*/
typedef struct doubleWordInfo 
{
	bool success;
	uint64_t data;
} doubleWordInfo_t;

/*
	Used when memory cannot be allocated.
*/
void allocationFailed();

/*
	Used to indicate the cache specs given are invalid.
*/
void invalidCache();

/*
	Used to indicate a physical memory file does not
	exist.
*/
void physicalMemFailed();

/*
	Creates a new cache with N ways that has a block size of blockDataSize,
	and a total data of size totalDataSize, both in Bytes. Also takes in a string
	which holds the name of a physical memory file and copys it into the
	cache. You CANNOT assume the pointer will remain valid in the function
	without copying. Returns a pointer to the cache. If any error occurs call
	the appropriate error function and return NULL.
*/ 
cache_t* createCache(uint32_t n, uint32_t blockDataSize, uint32_t totalDataSize, char* physicalMemoryName);

/*
	Function that frees all of the memory taken up by a cache.
*/
void deleteCache(cache_t* cache);

/*
	Takes in a memory address and the cache it will be written to and
	returns the value of the tag as the rightmost bits with leading
	0s.
*/
uint32_t getTag(cache_t* cache, uint32_t address); 

/*
	Takes in a memory address and the cache it will be written to and
	returns the value of the index as the rightmost bits with leading
	0s.
*/
uint32_t getIndex(cache_t* cache, uint32_t address);

/*
	Takes in a memory address and the cache it will be written to and
	returns the value of the offset as the rightmost bits with leading
	0s.
*/
uint32_t getOffset(cache_t* cache, uint32_t address);

/*
	Returns for a cache the number sets the cache contains.
*/
uint32_t getNumSets(cache_t* cache);

/*
	Given a cache returns the tag size in bits.
*/
uint8_t getTagSize(cache_t* cache);

/*
	Takes in a cache and determines the number of LRU bits the cache
	needs for each block.
*/
uint8_t numLRUBits(cache_t* cache);

/*
	Takes in a cache and returns the space it occupies in bits. 
*/
uint64_t cacheSizeBits(cache_t* cache);

/*
	Takes in a cache and returns how many bytes it should malloc.
*/
uint64_t cacheSizeBytes(cache_t* cache);

/*
	Calculates the number of garbage bits for the cache. Garbage Bits are
	extra bits that have to be malloced because we are forced to malloc
	a multiple of a byte. This will only be an issue for small caches but
	should always be accounted for.
*/
uint8_t numGarbageBits(cache_t* cache);

/*
	Returns the total number of bits a block takes up in a cache.
*/
uint64_t totalBlockBits(cache_t* cache);

/*
	Takes in a cache and a blocknumber and returns the value in
	bits that the block begins at.
*/
uint64_t getBlockStartBits(cache_t* cache, uint32_t blocknumber);

/*
	Takes in a block number and an address.
	Returns 1 if the tag constructed from the address equals the
	tag in the block specified and otherwise 0.
*/
int tagEquals(uint32_t blockNumber, uint32_t tag, cache_t* cache);

/* 
	Prints out the contents of the cache in this format. Each Cache will be
	formatted with 6 columns, the set number the valid bit, the dirty bit, 
	the LRU, the tag, and the data. Data and tag should be printed out in 
	hexadecimal. A horizontal line should begin and end the cache. Every 
	entry should be on a new line. A space and a verticle line should 
	separate each column in each row. A newline space should also be printed
	after each cache.
	EX:

	-----------------------------------------------
	set | valid | dirty | shared | LRU | tag | data
	0 | 1 | 1 | 0 | 0 | 0x6500 | 0x785237895239
	0 | 0 | 1 | 1 | 0 | 0x9568 | 0x87234fd72947
	1 | 1 | 1 | 1 | 0 | 0x7484 | 0x274024729074
	1 | 1 | 1 | 0 | 0 | 0x7481 | 0x736945236858
	-----------------------------------------------

*/
void printCache(cache_t* cache);

/*
	Takes in a number and determines if exactly one bit is turned on. Returns
	1 if there is 1 bit turned on and otherwise 0.
*/
int oneBitOn(uint32_t val);

/*
	Takes in a positive power of two and computes its logartihm base 2.
*/
uint8_t log_2(uint32_t val);

/*
	Takes in a cache and a block number and gets the location of the 
	of the valid bit in bits.
*/
uint64_t getValidLocation(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache and a block number and gets the location of the
	dirty bit in bits.
*/
uint64_t getDirtyLocation(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache and a block number and gets the location of the
	shared bit in bits.
*/
uint64_t getSharedLocation(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache and a block number and gets the location of the
	start of the LRU bits in bits.
*/
uint64_t getLRULocation(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache and a block number and gets the location of the
	start of the tag bits in bits.
*/
uint64_t getTagLocation(cache_t* cache, uint32_t blockNumber);

/*
	Takes in a cache, a block number, and an offset in bytes which can be 
	represented in the offset portion of the T:I:O and gets the location
	for the start of data section at that offset in bits.
*/
uint64_t getDataLocation(cache_t* cache, uint32_t blockNumber, uint32_t offset);

#endif