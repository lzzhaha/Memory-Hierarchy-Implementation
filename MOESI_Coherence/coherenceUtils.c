/* Summer 2017 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "coherenceUtils.h"
#include "../part1/utils.h"
#include "../part1/setInCache.h"
#include "../part1/getFromCache.h"

/*
	Used to indicate that a cache system has an invalid number
	of caches.
*/
void invalidCacheNumber() {
    fprintf(stderr, "\nError: Invalid number of caches\n");
}

/*
	Used to indicate that the cacheNodes or caches being passed in are null.
*/
void nullCacheError() {
	fprintf(stderr, "\nError: Cache(s) do not exist\n");
}

/*
	Used to indicated that the cache system contains duplicate
	ID numbers.
*/
void duplicateIDError() {
	fprintf(stderr, "\nError: Multiple caches with the same ID in the system\n");
}

/*
	Used to indicate that the cache system consists of nonidentical block sizes
	and therefore our cache system cannot be used.
*/
void blockSizeError() {
	fprintf(stderr, "\nError: Nonidentical blockSizes for caches in system\n");
}

/*
	Used to indicate that the cache system has no valid snooper
	and therefore our cache system cannot be used.
*/
void snooperError() {
	fprintf(stderr, "\nError: Snooper Invalid\n");
}

/*
	Used to indicate at least 1 cache has a different main memory location
	and as a result a set of caches that cannot constitute a system.
*/
void memError() {
	fprintf(stderr, "\nError: Caches are not in a system. At least 1 cache uses different main memory\n");
}

/*
	Function that takes in a pointer to a cache and an ID number and creates
	a cache node. You CAN assume that the cache has already been properly
	malloced.
*/
cacheNode_t* createCacheNode(cache_t* cache, uint8_t ID) {
	cacheNode_t* node = malloc(sizeof(cacheNode_t));
	if (node == NULL) {
		allocationFailed();
	}
	node->cache = cache;
	node->ID = ID;
	return node;
}

/*
	Function that creates a cache system. Takes in an array of cache
	nodes and a size and returns a pointer to the cache system.
	All caches must have the same block data size and no two caches can share
	an ID. In addition all caches in a system must share the same main memory
	source. IF any condition is failed call the appropriate error function
	and return NULL.
*/
cacheSystem_t* createCacheSystem(cacheNode_t** caches, uint8_t size, snoopy_t* snooper) {
	char* memFile;
	int ID;
	cache_t* cache;
	if (caches == NULL) {
		nullCacheError();
		return NULL;
	}
	if (size == 0) {
		invalidCacheNumber();
		return NULL;
	}
	if (snooper == NULL) {
		snooperError();
		return NULL;
	}
	int ID_Array[size];
	cache_t* cache_Array[size];
	if (caches[0] == NULL || caches[0]->cache == NULL) {
		nullCacheError();
		return NULL;
	}
	uint32_t blockDataSize = caches[0]->cache->blockDataSize;
	ID_Array[0] = caches[0]->ID;
	cache_Array[0] = caches[0]->cache;
	memFile = caches[0]->cache->physicalMemoryName;
	for (uint8_t i = 1; i < size; i++) {
		if (caches[i] == NULL || caches[i]->cache == NULL) {
			nullCacheError();
			return NULL;
		} else if (caches[i]->cache->blockDataSize != blockDataSize) {
			blockSizeError();
			return NULL;
		} else if (strcmp(caches[i]->cache->physicalMemoryName, memFile)) {
			memError();
			return NULL;
		} else {
			ID = caches[i]->ID;
			cache = caches[i]->cache;
			for (uint8_t j = 0; j < i; j++) {
				if (ID == ID_Array[j]) {
					duplicateIDError();
					return NULL;
				}
				if (cache == cache_Array[j]) {
					nullCacheError();
					return NULL;
				}
			}
			ID_Array[i] = ID;
			cache_Array[i] = cache;
		}
	}
	cacheSystem_t* sys = malloc(sizeof(cacheSystem_t));
	if (sys == NULL) {
		allocationFailed();
	}
	sys->caches = caches;
	sys->size = size;
	sys->blockDataSize = blockDataSize;
	sys->snooper = snooper;
	return sys;
}

/* 
	Takes in a cache system and frees it and any memory any of its parts take
	up. This is applied recursively.
*/
void deleteCacheSystem(cacheSystem_t* cacheSystem) {
	cacheNode_t* node;
	for (int i = 0; i < cacheSystem->size; i++) {
		node = cacheSystem->caches[i];
		deleteCache(node->cache);
		free(node);
	}
	free(cacheSystem->caches);
	deleteSnooper(cacheSystem->snooper);
	free(cacheSystem);
}

/*
	Takes in a cache system and an ID number and returns a pointer to the cache
	that is being stored in node with that ID number. If the ID number is not
	valid for the cache system then it returns a NULL pointer.
*/
cache_t* getCacheFromID(cacheSystem_t* cacheSystem, uint8_t ID) {
	cacheNode_t* node;
	uint8_t size = cacheSystem->size;
	for (unsigned int i = 0; i < size; i++) {
		node = cacheSystem->caches[i];
		if (node->ID == ID) {
			return node->cache;
		}
	}
	return NULL;
}

/*
	Takes in a cache and an address and determines the state of the block
	containing that address in the cache.
*/
enum state determineState(cache_t* cache, uint32_t address) {
	evictionInfo_t* info = findEviction(cache, address);
  if(info->match == false){
    free(info);
	  return INVALID;
  }
  else{
    uint32_t blockNumber = info->blockNumber;
    uint8_t valid = getValid(cache, blockNumber);
    uint8_t dirty = getDirty(cache, blockNumber);
    uint8_t shared = getShared(cache, blockNumber);
    if( ((!valid)&&(!dirty)&&(!shared)) || ((!valid)&&(!dirty)&&(shared)) 
                || ((!valid)&&(dirty)&&(!shared)) || ((!valid)&&(dirty)&&(shared)) ){
      free(info);
      return INVALID;
    }
    if( (valid)&&(!dirty)&&(!shared) ){
      free(info);
      return EXCLUSIVE;
    }
    if( (valid)&&(!dirty)&&(shared) ){
      free(info);
      return SHARED;
    }
    if( (valid)&&(dirty)&&(!shared) ){
      free(info);
      return MODIFIED;
    }
    if( (valid)&&(dirty)&&(shared) ){
      free(info);
      return OWNED;
    }
  }  
}

/*
	Function that takes in a cache, a block number, and a state to update to
	and updates the state of that block to be the desired new state.
*/
void setState(cache_t* cache, uint32_t blockNumber, enum state newState) {
	if(newState == INVALID)
    setValid(cache, blockNumber, 0);
  if(newState == EXCLUSIVE){
    setValid(cache, blockNumber, 1);
    setDirty(cache, blockNumber, 0);
    setShared(cache, blockNumber, 0);
  }
  if(newState == SHARED){
    setValid(cache, blockNumber, 1);
    setDirty(cache, blockNumber, 0);
    setShared(cache, blockNumber, 1);
  }
  if(newState == MODIFIED){
    setValid(cache, blockNumber, 1);
    setDirty(cache, blockNumber, 1);
    setShared(cache, blockNumber, 0);
  }
  if(newState == OWNED){
    setValid(cache, blockNumber, 1);
    setDirty(cache, blockNumber, 1);
    setShared(cache, blockNumber, 1);
  }
}

/*
	Takes in a cache, an address and a state that this address has in a different
	cache. Updates the state in our current cache based upon the state
	it gained in the other cache. This should only be called to update
	other caches after a read or write has been successfully performed on
	a cache.
*/
void updateState(cache_t* cache, uint32_t address, enum state otherState) {
	enum state curState = determineState(cache, address);
  uint32_t blockNumber;
  
  evictionInfo_t* info = findEviction(cache, address);
  if(info->match == false){
    free(info);
	  return;
  }
  else
    blockNumber = info->blockNumber;
    
    
  if(curState == INVALID){
    free(info);
    return;
  }
  if(curState == EXCLUSIVE){
    if(otherState == SHARED)
      setState(cache, blockNumber, SHARED);
    if(otherState == MODIFIED)
      setState(cache, blockNumber, INVALID);
  }
  if(curState == SHARED){
    if(otherState == MODIFIED)
      setState(cache, blockNumber, INVALID);
    if(otherState == INVALID)
      setState(cache, blockNumber, EXCLUSIVE);
  }
  if(curState == MODIFIED){
    if(otherState == SHARED)
      setState(cache, blockNumber, OWNED);
    if(otherState == MODIFIED)
      setState(cache, blockNumber, INVALID);
  }
  if(curState == OWNED){
    if(otherState == MODIFIED)
      setState(cache, blockNumber, INVALID);
    if(otherState == INVALID)
      setState(cache, blockNumber, MODIFIED);
  }
	free(info);
}

/*
	Creates a new snooper with 8 buckets.
*/
snoopy_t* createSnooper() {
	snoopy_t* snoopy = malloc(sizeof(snoopy_t));
	snoopy->buckets = malloc(sizeof(snoopBucket_t*) << 3);
	for (int i = 0; i < 8; i++) {
		snoopy->buckets[i] = createBucket();
	}
	snoopy->numBuckets = 8;
	snoopy->numContents = 0;
	return snoopy;
}

/*
	Takes in a snooper and deletes all the elements.
*/
void deleteSnooper(snoopy_t* snooper) {
	addressList_t* lst;
	addressList_t* nxtlst;
	snoopBucket_t* bucket;
	for (int i = 0; i < snooper->numBuckets; i++) {
		bucket = snooper->buckets[i];
		if (bucket->lst) {
			lst = bucket->lst;
			while (lst) {
				nxtlst = lst->next;
				free(lst);
				lst = nxtlst;
			}		
		}
		free(bucket);
	}
	free(snooper->buckets);
	free(snooper);
}

/*
	Creates a bucket for the snooper.
*/
snoopBucket_t* createBucket() {
	snoopBucket_t* bucket;
	bucket = malloc(sizeof(snoopBucket_t));
	bucket->lst = NULL;
	return bucket;
}

/*
	Creates a new list to keep track of IDs and addresses.
*/
addressList_t* createList(uint32_t address, uint8_t ID, addressList_t* next) {
	addressList_t* lst = malloc(sizeof(addressList_t));
	lst->address = address;
	lst->ID = ID;
	lst->next = next;
	return lst;
}

/*
	Hash function used to place addresses in a snooper.
*/
uint32_t hash(uint32_t address) {
    return (uint32_t) (((uint64_t) address * UINT32_C(2654435761)) >> 16); //Adapted from Knuth's multiplicative has function TAOCP volume 3 (2nd edition), section 6.4, page 516.
}

/*
	Adds a new list to the snooper. If a list with the same address and ID is already
	there it does nothing. Otherwise it finds its appropriate bucket for that address and
	appends it to the front of our list. If capacity becomes twice the number of buckets
	the number of buckets should shift.
*/
void addToSnooper(snoopy_t* snooper, uint32_t address, uint8_t ID, uint32_t blockDataSize) {
	uint32_t hashVal;
	address = address & ~(blockDataSize - 1);
	if (!snooperContains(snooper, address, ID)) {
		snooper->numContents++;
		if (snooper->numContents >= (snooper->numBuckets << 1)) {
			resizeSnooper(snooper);
		}
		hashVal = hash(address) & (snooper->numBuckets - 1);
		snooper->buckets[hashVal]->lst = createList(address, ID, snooper->buckets[hashVal]->lst);
	}
}

/*
	Takes in a snooper, an address, and an ID. Returns true if the snooper
	contains a list containing the address and ID.	
*/
bool snooperContains(snoopy_t* snooper, uint32_t address, uint8_t ID) {
	uint32_t hashVal;
	hashVal = hash(address) & (snooper->numBuckets - 1);
	addressList_t* lst = snooper->buckets[hashVal]->lst;
	while (lst) {
		if (lst->address == address && lst->ID == ID) {
			return true;
		}
		lst = lst->next;
	}
	return false;
}

/*
	Resizes the snooper if the load factor becomes >= 2.
*/
void resizeSnooper(snoopy_t* snooper) {
	uint32_t hashVal;
	snoopBucket_t* bucket;
	addressList_t* frontlst = NULL;
	addressList_t* lstMover = NULL;
	for (int i = 0; i < snooper->numBuckets; i++) {
		bucket = snooper->buckets[i];
		if (bucket->lst) {
			if (frontlst == NULL) {
				frontlst = bucket->lst;
				lstMover = bucket->lst;
			} else {
				lstMover->next = bucket->lst;
			}
			while (lstMover->next) {
				lstMover = lstMover->next;
			}
		}
		bucket->lst = NULL;
	}
	snooper->numBuckets = (snooper->numBuckets) << 1;
	snooper->buckets = realloc(snooper->buckets, sizeof(snoopBucket_t*) << log_2(snooper->numBuckets));
	for (int i = (snooper->numBuckets) >> 1; i < snooper->numBuckets; i++) {
		snooper->buckets[i] = createBucket();
	}
	while (frontlst) {
		lstMover = frontlst;
		frontlst = frontlst->next;
		hashVal = hash(lstMover->address) & (snooper->numBuckets - 1);
		lstMover->next = snooper->buckets[hashVal]->lst;
		snooper->buckets[hashVal]->lst = lstMover;
	}

}

/*
	Takes in a snooper, address, and block size and returns the ID of the cache
	 that contains the info if it is the sole cache. Otherwise it returns -1.
*/
int returnIDIf1(snoopy_t* snooper, uint32_t address, uint32_t blockDataSize) {
	int ID;
	uint8_t count;
	uint32_t hashVal;
	addressList_t* lst;
	address = address & ~(blockDataSize - 1);
	ID = -1;
	hashVal = hash(address) & (snooper->numBuckets - 1);
	lst = snooper->buckets[hashVal]->lst;
	count = 0;
	while (lst && count < 2) {
		if (address == lst->address) {
			count++;
			ID = lst->ID;
		}
		lst = lst->next;
	}
	return count < 2 ? ID : -1;
}

/*
	Takes in a snooper, an address, and a blocksize and returns the first 
	cache to contain the address. Returns -1 if there are none.
*/
int returnFirstCacheID(snoopy_t* snooper, uint32_t address, uint32_t blockDataSize) {
	uint32_t hashVal;
	address = address & ~(blockDataSize - 1);
	hashVal = hash(address) & (snooper->numBuckets - 1);
	addressList_t* lst = snooper->buckets[hashVal]->lst;
	while (lst) {
		if (address == lst->address) {
			return lst->ID;
		}
		lst = lst->next;
	}
	return -1;
}

/*
	Takes in an address and an ID and removes that list content from the table.
	If the contents are not in the table it does nothing.
*/
void removeFromSnooper(snoopy_t* snooper, uint32_t address, uint8_t ID, uint32_t blockDataSize) {
	uint32_t hashVal;
	address = address & ~(blockDataSize - 1);
	hashVal = hash(address) & (snooper->numBuckets - 1);
	removeItem(&(snooper->buckets[hashVal]->lst), address, ID);
}

/*
	Takes in a pointer to a pointer to a list, an address, and an ID and removes
	the list element from the list that matches the ID. If no element matches
	it does nothing.
*/
void removeItem(addressList_t** lst, uint32_t address, uint8_t ID) {
	if (lst == NULL || *lst == NULL) {
		return;
	}
	addressList_t* temp = *lst;
	if (temp->address == address && temp->ID == ID) {
		*lst = temp->next;
		free(temp);
	} else {
		return removeItem(&(temp->next), address, ID);
	}
}

/*
	Decrements the LRU of every block by 1 except for the block that just
	got invalidated which is set to the LRU max value.
*/
void decrementLRU(cache_t* cache, uint32_t tag, uint32_t idx, long oldLRU) {
	int currLRU;
	uint32_t blockNumber;
	uint32_t blockNumberStart = idx << log_2(cache->n);
	for (int i = 0; i < cache->n; i++) {
		blockNumber = blockNumberStart + i;
		if (tagEquals(blockNumber, tag, cache)) {
			setLRU(cache, blockNumber, cache->n - 1);
		} else if (getValid(cache, blockNumber)) {
			currLRU = getLRU(cache, blockNumber);
			if (currLRU > oldLRU) {
			setLRU(cache, blockNumber, currLRU - 1);
			}
		}
	}
}
