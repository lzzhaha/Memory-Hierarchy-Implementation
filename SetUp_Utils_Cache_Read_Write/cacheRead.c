/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "setInCache.h"
#include "cacheRead.h"
#include "cacheWrite.h"
#include "getFromCache.h"
#include "mem.h"
#include "../part2/hitRate.h"

/*
	Takes in a cache and a block number and fetches that block of data, 
	returning it in a uint8_t* pointer.
*/
uint8_t* fetchBlock(cache_t* cache, uint32_t blockNumber) {
	uint64_t location = getDataLocation(cache, blockNumber, 0); 
	uint32_t length = cache->blockDataSize;
	uint8_t* data = malloc(sizeof(uint8_t) << log_2(length));
	if (data == NULL) {
		allocationFailed();
	}
	int shiftAmount = location & 7;
	uint64_t byteLoc = location >> 3;
	if (shiftAmount == 0) {
		for (uint32_t i = 0; i < length; i++) {
			data[i] = cache->contents[byteLoc + i];
		}
	} else {
		length = length << 3;
		data[0] = cache->contents[byteLoc] << shiftAmount;
		length -= (8 - shiftAmount);
		int displacement = 1;
		while (length > 7) {
			data[displacement - 1] = data[displacement - 1] | (cache->contents[byteLoc + displacement] >> (8 - shiftAmount));
			data[displacement] = cache->contents[byteLoc + displacement] << shiftAmount;
			displacement++;
			length -= 8;
		}
		data[displacement - 1] = data[displacement - 1] | (cache->contents[byteLoc + displacement] >> (8 - shiftAmount));
	}
	return data;
}

/*
	Takes in a cache, an address, and a dataSize and reads from the cache at
	that address the number of bytes indicated by the size. If the data block 
	is already in the cache it retrieves the contents. If the contents are not
	in the cache it is read into a new slot and if necessary something is 
	evicted.
*/
uint8_t* readFromCache(cache_t* cache, uint32_t address, uint32_t dataSize) {
	//Validate the address and data size
	if(!validAddresses(address,dataSize))
		{return NULL;}
	uint32_t edge_mark = 0;//track the conjunction of each segment of data if it is spread over multiple blocks
        uint8_t* data = malloc(dataSize);
	
        while(true){//Accessing each different block for one iteration
              	reportAccess(cache);
		evictionInfo_t* evictionBlock = findEviction(cache,address);
		uint32_t blockNumber = evictionBlock->blockNumber;
		uint32_t tag = extractTag(cache,blockNumber);
		uint32_t index = extractIndex(cache,blockNumber);
		uint32_t LRU = evictionBlock->LRU;
		uint32_t offset = address%(cache->blockDataSize);
		if(evictionBlock->match){//cache hit
			reportHit(cache);
			updateLRU(cache,tag,index,LRU);
			uint64_t dataLocation = getDataLocation(cache,blockNumber,offset);
	                uint64_t next_block_start = getBlockStartBits(cache,blockNumber+1);
			if ((dataSize-1)*8+ dataLocation < next_block_start){//data fit into the block
                                uint8_t* temp_data = getData(cache,offset,blockNumber,dataSize);
                                //append the obtained data into the returned array
                                for(uint32_t i=0;i<dataSize;i++){
                                        data[edge_mark++]=temp_data[i];
                                }
                                free(temp_data);
				free(evictionBlock);
                                break;
			}else{//data spread over multiple blocks
                                //load data till the end of block
                                uint32_t segment_length = (next_block_start-dataLocation)/8;
                                uint8_t* temp_data = getData(cache,offset,blockNumber,segment_length);
                                //append the obtained data into the returned array
                                for(uint32_t i=0;i<segment_length;i++){
                                        data[edge_mark++]=temp_data[i];
                                }
                                free(temp_data);
                                // keep searching in next loop
                                dataSize-=segment_length;
                                address+=segment_length;
				free(evictionBlock);
                                continue;
			}
		}else{//cache miss
			uint32_t dataBlock_start = address-offset;
		        uint8_t* new_block = readFromMem(cache,dataBlock_start);
		        evict(cache,blockNumber);
                        setData(cache,new_block,blockNumber,cache->blockDataSize,0);
                        free(new_block);
                        new_block = NULL;
			uint32_t new_tag = getTag(cache,address);
                        setValid(cache,blockNumber,1);
                        setTag(cache,new_tag,blockNumber);
                        setShared(cache,blockNumber,0);
                        setDirty(cache,blockNumber,0);
                        updateLRU(cache,new_tag,index,LRU);
			
			//use the previous algorithm to get the data, subtract access and hit so as to maintain the correct hit rate
			cache->access--;
			cache->hit--;
			free(evictionBlock);
			continue;	
		}
	}	
/*
                 if (tagEquals(blockNumber,cache,tag)&& valid){//cache hit
			reportHit(cache);
                        updateLRU(cache, tag,index,getLRU(cache,blockNumber));
                        if ((dataSize-1)*8+ dataLocation < next_block_start){//data fit into the block
                                uint8_t* temp_data = getData(cache,offset,blockNumber,dataSize);
                                //append the obtained data into the returned array
                                for(uint32_t i=0;i<dataSize;i++){
                                        data[edge_mark++]=temp_data[i];
                                }
				free(temp_data);
                                break;
                        }else{//data spread over multiple blocks
                                block_order_in_set =0;
                                //load data till the end of block
                                uint32_t segment_length = (next_block_start-dataLocation)/8;
                                uint8_t* temp_data = getData(cache,offset,blockNumber,segment_length);
                                //append the obtained data into the returned array
                                for(uint32_t i=0;i<segment_length;i++){
                                        data[edge_mark++]=temp_data[i];
                                }
				free(temp_data);
                                // keep searching in next loop
                                dataSize-=segment_length;
                                address+=segment_length;
                                continue;

                        }
                }else if(block_order_in_set<n-1){//search in the set
                        block_order_in_set++;
                        continue;
                }else{//cache miss
			
                        //Search for the block with maximum LRU to replace
                        for(uint32_t i=0;i<n;i++){
                                blockNumber = n*index*blockDataSize+i;
                                if(getLRU(cache,blockNumber)==n-1){
                                        //replace the block and update LRU
                                        uint8_t* new_block = readFromMem(address);
                                        setData(cache,new_block,blockNumber,blockDataSize,0);
                                        setValid(cache,blockNumber,1);
                                        setTag(cache,tag,blockNumber);
                                        //Shared bit? setShared(cache,blockNumber,0);
                                        setDirty(cache,blockNumber,0);
                                        updateLRU(cache,tag,index,n-1);
                                        block_order_in_set = i;
                                        break;
                                }
                        }
			evictionInfo_t* evicInfo = findEviction(cache,address);
			blockNumber = evicInfo->blockNumber;
         	        uint8_t* new_block = readFromMem(address);
			//if the replacement block is dirty and valid, update the corresponding memory content
			if(getDirty(cache,blockNumber)&&getValid(cache,blockNumber)){
				uint32_t replace_tag = extractTag(cache,blockNumber);
				uint8_t shift_amount = off_bits + in_bits;
				replace_tag = replace_tag<<shift_amount;
				uint32_t replace_index= index<<off_bits;
				writeToMem(cache,blockNumber,replace_tag+replace_index);
				setDirty(cache,blockNumber,0);
			}
			evict(cache,blockNumber);
                        setData(cache,new_block,blockNumber,blockDataSize,0);
			free(new_block);
			new_block = NULL;
                        setValid(cache,blockNumber,1);
                        setTag(cache,tag,blockNumber);
			setShared(cache,blockNumber,0);
                        setDirty(cache,blockNumber,0);
                        updateLRU(cache,tag,index,evicInfo->LRU);
                        block_order_in_set = blockNumber-(index<<log_2(n));
                }

*/                                                                            
	return data;
}

/*
	Takes in a cache and an address and fetches a byte of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
byteInfo_t readByte(cache_t* cache, uint32_t address) {
	byteInfo_t retVal;
	uint8_t* ret = readFromCache(cache,address,1);
	if(ret){
		retVal.success = true;
		retVal.data = *ret;
	}else{
		retVal.success = false;
		retVal.data = 0;
	}
	free(ret);
	return retVal;
}

/*
	Takes in a cache and an address and fetches a halfword of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
halfWordInfo_t readHalfWord(cache_t* cache, uint32_t address) {
	halfWordInfo_t retVal;
	//check address alignment
	if(address%2){
		retVal.success = false;	
		return retVal;
	}
	uint8_t* ret = readFromCache(cache,address,2);
        if(ret){
                retVal.success = true;
                retVal.data = (((uint16_t)ret[0])<<8)+((uint16_t)ret[1]);
        }else{
                retVal.success = false;
                retVal.data = 0;
        }
	free((uint16_t*)ret);
	return retVal;
}

/*
	Takes in a cache and an address and fetches a word of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
wordInfo_t readWord(cache_t* cache, uint32_t address) {
	wordInfo_t retVal;
	if(address%4){
                retVal.success = false;
                return retVal;
        }
	halfWordInfo_t upper = readHalfWord(cache,address);
	uint32_t upper_data =((uint32_t)upper.data)<<16;
	if(cache->blockDataSize>=4){
		cache->access--;
		cache->hit--;
	}
	halfWordInfo_t lower = readHalfWord(cache,address+2);
	uint32_t lower_data = (uint32_t)lower.data;
        if(upper.success && lower.success){
                retVal.success = true;
                retVal.data = upper_data+lower_data;
        }else{
                retVal.success = false;
                retVal.data = 0;
        }

	return retVal;
}

/*
	Takes in a cache and an address and fetches a double word of data.
	Returns a struct containing a bool field of whether or not
	data was successfully read and the data. This field should be
	false only if there is an alignment error or there is an invalid
	address selected.
*/
doubleWordInfo_t readDoubleWord(cache_t* cache, uint32_t address) {
	doubleWordInfo_t retVal;
	if(address%8){
                retVal.success = false;
                return retVal;
        }
        wordInfo_t upper = readWord(cache,address);
	uint64_t upper_data = ((uint64_t)upper.data)<<32;
	 if(cache->blockDataSize>=8){
                cache->access--;
                cache->hit--;
        }
	wordInfo_t lower = readWord(cache,address+4);
	uint64_t lower_data = (uint64_t)lower.data;
	if(upper.success&&lower.success){
                retVal.success = true;
                retVal.data = upper_data + lower_data;
        }else{
                retVal.success = false;
                retVal.data = 0;
        }

	return retVal;
}
