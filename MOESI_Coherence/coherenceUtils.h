/* Summer 2017 */
#ifndef COHERENCEUTILS_H
#define COHERENCEUTILS_H
#include <stdbool.h>
#include <stdint.h>
#include "../part1/utils.h"

/*
	Enum used to sepcify the various allowed state in the MOESI coherence
	system.
*/
enum state {MODIFIED, OWNED, EXCLUSIVE, SHARED, INVALID};

/*
	Struct used to contain an individual cache for a coherent system. Consists
	of a pointer to a cache and an ID.
*/
typedef struct cacheNode {
	cache_t* cache;
	uint8_t ID;
} cacheNode_t;

/*
	Struct used to contain a list of all addresses and the caches in which
	they lie. Is a linked list.
*/
typedef struct addressList {
	uint32_t address;
	uint8_t ID;
	struct addressList* next;
} addressList_t;

/*
	Stuct used to represent each bucket in the snooper. Contains a pointer to
	a list with an address.
*/
typedef struct snoopBucket{
	addressList_t* lst;
} snoopBucket_t;

/*
	Struct used to create the snooper for each cache. Each cache should contain
	exactly one snooper. The buckets are used to control the actual mapping.
	The  numContents is the number of elements in the table. If numContents
	is twice the numBuckets the number of buckets should double. This table
	will not dynamically shrink.
*/
typedef struct snoopy{
	snoopBucket_t** buckets;
	uint8_t numBuckets;
	uint8_t numContents;
} snoopy_t;

/*
	Struct used to contain a network of coherent caches. Consists of a
	double pointer to cache nodes, a size of the network, and the blockDataSize
	for the cacehe. All caches must have the same block data size and each have
	unique IDs.
*/
typedef struct cacheSystem{
	cacheNode_t** caches;
	uint8_t size;
	uint32_t blockDataSize;
	snoopy_t* snooper;
} cacheSystem_t;

/*
	Used to indicate that a cache system has an invalid number
	of caches.
*/
void invalidCacheNumber();

/*
	Used to indicate that the cacheNodes or caches being passed in are null.
*/
void nullCacheError();

/*
	Used to indicated that the cache system contains duplicate
	ID numbers.
*/
void duplicateIDError();

/*
	Used to indicate that the cache system consists of nonidentical block sizes
	and therefore our cache system cannot be used.
*/
void blockSizeError();

/*
	Used to indicate that the cache system has no valid snooper
	and therefore our cache system cannot be used.
*/
void snooperError();

/*
	Used to indicate at least 1 cache has a different main memory location
	and as a result a set of caches that cannot constitute a system.
*/
void memError();

/*
	Function that takes in a pointer to a cache and a an ID number and creates
	a cache node. You CAN assume that the cache has already been properly
	malloced.
*/
cacheNode_t* createCacheNode(cache_t* cache, uint8_t ID);

/*
	Function that creates a cache system. Takes in an array of cache
	nodes and a size and returns a pointer to the cache system.
	All caches must have the same block data size and no two caches can share
	an ID. In addition all caches in a system must share the same main memory
	source. IF any condition is failed call the appropriate error function
	and return NULL.
*/
cacheSystem_t* createCacheSystem(cacheNode_t** caches, uint8_t size, snoopy_t* snooper);

/* 
	Takes in a cache system and frees it and any memory any of its parts take
	up. This is applied recursively.
*/
void deleteCacheSystem(cacheSystem_t* cacheSystem);

/*
	Takes in a cache system and an ID number and returns a pointer to the cache
	that is being stored in node with that ID number. If the ID number is not
	valid for the cache system then it returns a NULL pointer.
*/
cache_t* getCacheFromID(cacheSystem_t* cacheSystem, uint8_t ID);

/*
	Takes in a cache and an address and determines the state of the block
	containing that address in the cache.
*/
enum state determineState(cache_t* cache, uint32_t address);

/*
	Function that takes in a cache, a block number, and a state to update to
	and updates the state of that block to be the desired new state.
*/
void setState(cache_t* cache, uint32_t blockNumber, enum state newState);


/*
	Takes in a cache an address and a state that his address has in a different
	cache. Updates the state in our current cache based upon the state
	it gained in the other cache. This should only be called to update
	other caches after a read or write has been successfully performed on
	a cache.
*/
void updateState(cache_t* cache, uint32_t address, enum state otherState);

/*
	Creates a new snooper with 8 buckets.
*/
snoopy_t* createSnooper();

/*
	Takes in a snooper and deletes all the elements.
*/
void deleteSnooper(snoopy_t* snooper);

/*
	Creates a bucket for the snooper.
*/
snoopBucket_t* createBucket();

/*
	Creates a new list to keep track of IDs and addresses.
*/
addressList_t* createList(uint32_t address, uint8_t ID, addressList_t* next);

/*
	Hash function used to place addresses in a snooper.
*/
uint32_t hash(uint32_t address);

/*
	Adds a new list to the snooper. If a list with the same address and ID is already
	there it does nothing. Otherwise it finds its appropriate bucket for that address and
	appends it to the front of our list. If capacity becomes twice the number of buckets
	the number of buckets should shift.
*/
void addToSnooper(snoopy_t* snooper, uint32_t address, uint8_t ID, uint32_t blockDataSize);

/*
	Takes in a snooper, an address, and an ID. Returns true if the snooper
	contains a list containing the address and ID.	
*/
bool snooperContains(snoopy_t* snooper, uint32_t address, uint8_t ID);

/*
	Resizes the snooper if the load factor becomes >= 2.
*/
void resizeSnooper(snoopy_t* snooper);

/*
	Takes in a snooper, address, and block size and returns the ID of the cache
	 that contains the info if it is the sole cache. Otherwise it returns -1.
*/
int returnIDIf1(snoopy_t* snooper, uint32_t address, uint32_t blockDataSize);

/*
	Takes in a snooper, an address, and a blocksize and returns the first 
	cache to contain the address. Returns -1 if there are none.
*/ 
int returnFirstCacheID(snoopy_t* snooper, uint32_t address, uint32_t blockDataSize);

/*
	Takes in an address and an ID and removes that list content from the table.
	If the contents are not in the table it does nothing.
*/
void removeFromSnooper(snoopy_t* snooper, uint32_t address, uint8_t ID, uint32_t blockDataSize);

/*
	Takes in a pointer to a pointer to a list, an address, and an ID and removes
	the list element from the list that matches the ID. If no element matches
	it does nothing.
*/
void removeItem(addressList_t** lst, uint32_t address, uint8_t ID);

/*
	Decrements the LRU of every block by 1 except for the block that just
	got invalidated which is set to the LRU max value.
*/
void decrementLRU(cache_t* cache, uint32_t tag, uint32_t idx, long oldLRU);
#endif