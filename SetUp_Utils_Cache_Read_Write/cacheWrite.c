/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "cacheWrite.h"
#include "getFromCache.h"
#include "mem.h"
#include "setInCache.h"
#include "../part2/hitRate.h"

/*
	Takes in a cache and a block number and evicts the block at that number
	from the cache. This does not change any of the bits in the cache but 
	checks if data needs to be written to main memory or and then makes 
	calls to the appropriate functions to do so.
*/
void evict(cache_t* cache, uint32_t blockNumber) {
	uint8_t valid = getValid(cache, blockNumber);
	uint8_t dirty = getDirty(cache, blockNumber);
	if (valid && dirty) {
		uint32_t tag = extractTag(cache, blockNumber);
		uint32_t address = extractAddress(cache, tag, blockNumber, 0);
		writeToMem(cache, blockNumber, address);
	}
}

/*
	Takes in a cache, an address, a pointer to data, and a size of data
	and writes the updated data to the cache. If the data block is already
	in the cache it updates the contents and sets the dirty bit. If the
	contents are not in the cache it is written to a new slot and 
	if necessary something is evicted from the cache.
*/
void writeToCache(cache_t* cache, uint32_t address, uint8_t* data, uint32_t dataSize) {
	if(!validAddresses(address,dataSize))
	{return;}
        uint32_t edge_mark = 0;//track the conjunction of each segment of data if it is spread over multiple blocks
        uint32_t blockNumber =0;
	while(true){
               	reportAccess(cache); 
		evictionInfo_t* evictionBlock = findEviction(cache,address);
                blockNumber = evictionBlock->blockNumber;
                uint32_t tag = extractTag(cache,blockNumber);
                uint32_t index = extractIndex(cache,blockNumber);
                uint32_t LRU = evictionBlock->LRU;
		uint32_t offset = address%(cache->blockDataSize);		
                uint64_t dataLocation = getDataLocation(cache,blockNumber,offset);
                uint64_t next_block_start = getBlockStartBits(cache,blockNumber+1);

                 if (evictionBlock->match){//cache write hit
                       	reportHit(cache); 
			updateLRU(cache, tag,index,getLRU(cache,blockNumber));
			setDirty(cache,blockNumber,1);
                        setShared(cache,blockNumber,0);
                        if ((dataSize-1)*8+ dataLocation < next_block_start){//data fit into the block
                                setData(cache,data+edge_mark,blockNumber,dataSize,offset);
				free(evictionBlock);
                                break;
                        }else{//data spread over multiple blocks
                                //write data till the end of block
                                uint32_t segment_length = (next_block_start-dataLocation)/8;
                                setData(cache,data+edge_mark,blockNumber,segment_length,offset);
                                edge_mark+=segment_length;
                                // keep searching in next loop
                                dataSize-=segment_length;
                                address+=segment_length;
				free(evictionBlock);
                                continue;

                        }
                }else{//cache miss
			evict(cache,blockNumber);
			uint32_t dataBlock_start = address-offset;
			uint8_t* new_block = readFromMem(cache,dataBlock_start);
		        setData(cache,new_block,blockNumber,cache->blockDataSize,0);
			free(new_block);
			new_block = NULL;
                        setValid(cache,blockNumber,1);
			uint32_t new_tag = getTag(cache,address);
                        setTag(cache,new_tag,blockNumber);
                        setShared(cache,blockNumber,0);
                        setDirty(cache,blockNumber,0);
                        updateLRU(cache,new_tag,index,LRU);

                        //use the previous algorithm to write the data, subtract access and hit so as to maintain the correct hit rate
                        cache->access--;
                        cache->hit--;
			free(evictionBlock);
                        continue;
	
                }
        }

}

/*
	Takes in a cache, an address to write to, a pointer containing the data
	to write, the size of the data, a tag, and a pointer to an evictionInfo
	struct and writes the data given to the cache based upon the location
	given by the evictionInfo struct.
*/
void writeDataToCache(cache_t* cache, uint32_t address, uint8_t* data, uint32_t dataSize, uint32_t tag, evictionInfo_t* evictionInfo) {
	uint32_t idx = getIndex(cache, address);
	setData(cache, data, evictionInfo->blockNumber, dataSize , getOffset(cache, address));
	setDirty(cache, evictionInfo->blockNumber, 1);
	setValid(cache, evictionInfo->blockNumber, 1);
	setShared(cache, evictionInfo->blockNumber, 0);
	updateLRU(cache, tag, idx, evictionInfo->LRU);
}

/*
	Takes in a cache, an address, and a byte of data and writes the byte
	of data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns -1
	if the address is invalid, otherwise 0.
*/
int writeByte(cache_t* cache, uint32_t address, uint8_t data) {
	if(!validAddresses(address,1)){
		return -1;
	}
	writeToCache(cache,address,&data,1);
	return 0;
}

/*
	Takes in a cache, an address, and a halfword of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid
	address was used.
*/
int writeHalfWord(cache_t* cache, uint32_t address, uint16_t data) {
	if(!validAddresses(address,2)||(address%2)){
                return -1;
        }
	uint8_t upper = (uint8_t)(data>>8);
	uint8_t lower = (uint8_t)(data&255);
	writeByte(cache,address,upper);
	if(cache->blockDataSize>=2){
                cache->access--;
                cache->hit--;
        }
	writeByte(cache,address+1,lower);
	return 0;
}

/*
	Takes in a cache, an address, and a word of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid
	address was used.
*/
int writeWord(cache_t* cache, uint32_t address, uint32_t data) {
	if(!validAddresses(address,4)||(address%4)){
                return -1;
        }
        uint16_t upper = (uint16_t)(data>>16);
	uint32_t mask = ((uint32_t)upper)<<16;
        uint16_t lower = (uint16_t)(data-mask);
        writeHalfWord(cache,address,upper);
        if(cache->blockDataSize>=4){
                cache->access--;
                cache->hit--;
        }

	writeHalfWord(cache,address+2,lower);

	return 0;
}

/*
	Takes in a cache, an address, and a double word of data and writes the
	data to the cache. May evict something if the block is not already
	in the cache which may also require a fetch from memory. Returns 0
	for a success and -1 if there is an allignment error or an invalid address
	was used.
*/
int writeDoubleWord(cache_t* cache, uint32_t address, uint64_t data) {
	if(!validAddresses(address,8)||address%8){
                return -1;
        }
        uint32_t upper = (uint32_t)(data>>32);
        uint64_t mask = ((uint64_t)upper)<<32;
        uint32_t lower = (uint32_t)(data-mask);
        writeWord(cache,address,upper);
        if(cache->blockDataSize>=8){
                cache->access--;
                cache->hit--;
        }

	writeWord(cache,address+4,lower);

	return 0;
}

/*
	A function used to write a whole block to a cache without pulling it from
	physical memory. This is useful to transfer information between caches
	without needing to take an intermediate step of going through main memory,
	a primary advantage of MOESI. Takes in a cache to write to, an address
	which is being written to, the block number that the data will be written
	to and an entire block of data from another cache.
*/
void writeWholeBlock(cache_t* cache, uint32_t address, uint32_t evictionBlockNumber, uint8_t* data) {
	uint32_t idx = getIndex(cache, address);
	uint32_t tagVal = getTag(cache, address);
	int oldLRU = getLRU(cache, evictionBlockNumber);
	evict(cache, evictionBlockNumber);
	setValid(cache, evictionBlockNumber, 1);
	setDirty(cache, evictionBlockNumber, 0);
	setTag(cache, tagVal, evictionBlockNumber);
	setData(cache, data, evictionBlockNumber, cache->blockDataSize, 0);
	updateLRU(cache, tagVal, idx, oldLRU);
}
