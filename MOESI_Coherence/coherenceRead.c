/* Summer 2017 */
#include <stdbool.h>
#include <stdint.h>
#include "coherenceUtils.h"
#include "coherenceRead.h"
#include "../part1/utils.h"
#include "../part1/setInCache.h"
#include "../part1/getFromCache.h"
#include "../part1/cacheRead.h"
#include "../part1/cacheWrite.h"
#include "../part1/mem.h"
#include "../part2/hitRate.h"

/*
	A function which processes all cache reads for an entire cache system.
	Takes in a cache system, an address, an id for a cache, and a size to read
	and calls the appropriate functions on the cache being selected to read
	the data. Returns the data.
*/
uint8_t* cacheSystemRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t size) {
	 if(!validAddresses(address,size)||!cacheSystem||size<=0)
                {return NULL;}
	uint8_t* retVal = malloc(size);
	uint8_t offset =0 ;
	//uint8_t* transferData;
	evictionInfo_t* dstCacheInfo;
//	evictionInfo_t* otherCacheInfo;
	uint32_t evictionBlockNumber;
	cacheNode_t** caches;
//	bool otherCacheContains = false;
	cache_t* dstCache = NULL;
	uint8_t counter = 0;
	caches = cacheSystem->caches;
	uint32_t tag = 0;
	uint32_t index =0;
	while (dstCache == NULL && counter < cacheSystem->size) { //Selects destination cache pointer from array of caches pointers
		if (caches[counter]->ID == ID) {
			dstCache = caches[counter]->cache;
			offset = getOffset(dstCache,address);
		}
		counter++;
	}
	if(!dstCache){//Invalid ID
		return NULL;
	}
	dstCacheInfo = findEviction(dstCache, address); //Finds block to evict and potential match
	evictionBlockNumber = dstCacheInfo->blockNumber;
	tag = extractTag(dstCache,evictionBlockNumber);
        uint64_t dataLocation = getDataLocation(dstCache,evictionBlockNumber,offset);
	index = extractIndex(dstCache,evictionBlockNumber);
	
	if (dstCacheInfo->match) {
		//read own Cache Hit 
        	uint64_t next_block_start = getBlockStartBits(dstCache,evictionBlockNumber+1);
		if((size-1)*8+ dataLocation < next_block_start){//if the data is within the block
			  retVal = getData(dstCache,offset,evictionBlockNumber,size);
                          //append the obtained data into the returned array
                      	  updateLRU(dstCache,tag,index,dstCacheInfo->LRU);
		}else{//data spread over multiple blocks
                                //load data till the end of block
                                uint32_t segment_length = (next_block_start-dataLocation)/8;
                                uint8_t* temp_data = getData(dstCache,offset,evictionBlockNumber,segment_length);
                                //append the obtained data into the returned array
                                for(uint32_t i=0;i<segment_length;i++){
                                        retVal[i]=temp_data[i];
                                }
	                        updateLRU(dstCache,tag,index,dstCacheInfo->LRU);
                                free(temp_data);
                                // keep searching in next loop
                                size-=segment_length;
                                address+=segment_length;
				//recursively reading the data
				temp_data =cacheSystemRead(cacheSystem,address,ID,size);
				for(uint32_t i=0;i<size;i++){
                                        retVal[segment_length+i]=temp_data[i];
                                }
		}				

	} else {//The block is not in the current cache
		uint32_t oldAddress = extractAddress(dstCache, extractTag(dstCache, evictionBlockNumber), evictionBlockNumber, 0);
		/*How do you need to update the snooper?*/
		/*How do you need to update states for what is getting evicted (don't worry about evicting this will be handled at a later step when you place data in the cache)?*/
		/*Your Code Here*/
		removeFromSnooper(cacheSystem->snooper,oldAddress,ID,cacheSystem->blockDataSize);
		int remaining_cache_ID =0;
		switch(determineState(dstCache,oldAddress)){
			case OWNED:
				evict(dstCache,evictionBlockNumber);
			        remaining_cache_ID = returnIDIf1(cacheSystem->snooper,oldAddress,cacheSystem->blockDataSize);
				if(remaining_cache_ID !=-1){
					//there is only one cache contatinin the address now, so set it to EXCLUSIVE
            				cache_t* remaining_cache = getCacheFromID(cacheSystem,remaining_cache_ID);
                        		evictionInfo_t* remain_evict = findEviction(remaining_cache,oldAddress);
	                        	uint32_t block_to_set = remain_evict->blockNumber;
        	                	setState(remaining_cache,block_to_set,EXCLUSIVE);
                		}
				break;
			case SHARED:
				remaining_cache_ID = returnIDIf1(cacheSystem->snooper,oldAddress,cacheSystem->blockDataSize);
                                if(remaining_cache_ID !=-1){
                                        //there is only one cache contatinin the address now, so set it to EXCLUSIVE or MODIFIED
                                        cache_t* remaining_cache = getCacheFromID(cacheSystem,remaining_cache_ID);
                                        evictionInfo_t* remain_evict = findEviction(remaining_cache,oldAddress);
                                        uint32_t block_to_set = remain_evict->blockNumber;
					enum state next_state = (getDirty(remaining_cache,block_to_set))?MODIFIED:EXCLUSIVE;
                                        setState(remaining_cache,block_to_set,next_state);
                                }
                                break;
			default:
                                evict(dstCache,evictionBlockNumber);
                                break;

		}
		

			
		int otherCacheID = returnFirstCacheID(cacheSystem->snooper, address, cacheSystem->blockDataSize);
		/*Check other caches???*/
		/*Your Code Here*/
		if (otherCacheID==-1) {//no other cache contains the address
			/*Check Main memory?*/
			/*Your Code Here*/
			uint8_t* new_block = readFromMem(dstCache,address);
                        setData(dstCache,new_block,evictionBlockNumber,cacheSystem->blockDataSize,0);
                        free(new_block);
                        new_block = NULL;
			uint32_t new_tag = getTag(dstCache,address);
                        setValid(dstCache,evictionBlockNumber,1);
                        setTag(dstCache,new_tag,evictionBlockNumber);
                        updateLRU(dstCache,new_tag,index,dstCacheInfo->LRU);
			retVal = cacheSystemRead(cacheSystem,address,ID,size);
			setState(dstCache,evictionBlockNumber,EXCLUSIVE);
		}else{//other cache contains the address
			//Check the state of the first cache in the list which contains the address
			cache_t* firstCache = getCacheFromID(cacheSystem,(uint8_t)otherCacheID);
			evictionInfo_t* firstCacheInfo = findEviction(firstCache,address);
			uint32_t firstCache_blockNumber = firstCacheInfo->blockNumber;
			free(firstCacheInfo);
			uint8_t* data_from_other_block = getData(firstCache,0,firstCache_blockNumber
                                ,cacheSystem->blockDataSize);
			//copy the block from other cache
			uint32_t new_tag = getTag(dstCache,address);
                        writeWholeBlock(dstCache, address,evictionBlockNumber,data_from_other_block);
			setTag(dstCache,new_tag,evictionBlockNumber);
                        updateLRU(dstCache,new_tag,index,dstCacheInfo->LRU);
			setState(dstCache,evictionBlockNumber,SHARED);
			if(!getShared(firstCache,firstCache_blockNumber)){
				/*The first Cache in the list is not in shared state,
				which means that some changes need to be made for 
				the caches which already have the address in the list
				*/
				if(getDirty(firstCache,firstCache_blockNumber)){
					//the first Cache is modified state, change it to owned
					setState(firstCache,firstCache_blockNumber,OWNED);
				}else{//the first Cache is exclusive stat
					setState(firstCache,firstCache_blockNumber,SHARED);
				}
			
			}
      			free(data_from_other_block);
			 retVal = getData(dstCache,offset,evictionBlockNumber,size);
		}

	}
	addToSnooper(cacheSystem->snooper, address, ID, cacheSystem->blockDataSize);
	free(dstCacheInfo);
	return retVal;


}

/*
	A function used to request a byte from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
byteInfo_t cacheSystemByteRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	byteInfo_t retVal;
	uint8_t* data;
	retVal.success = true;
	data = cacheSystemRead(cacheSystem, address, ID, 1);
	//if (data == NULL) {
		//retVal.success = false;
		//retVal.data = 0;
		
	//}else{
		//retVal.data = data[0];
	//}
  //free
	//return retVal;
  if (data == NULL) {
		return retVal;
	}
	retVal.data = data[0];
	free(data);
	return retVal;
}

/*
	A function used to request a halfword from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
halfWordInfo_t cacheSystemHalfWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	//byteInfo_t temp;
	halfWordInfo_t retVal;
	uint8_t* data;
	retVal.success = true;
	/* Error Checking??*/
	if(address%2){
		retVal.success = false;
		return retVal;	
	}
	byteInfo_t temp;
	if (cacheSystem->blockDataSize < 2) {
		temp = cacheSystemByteRead(cacheSystem, address, ID);
		retVal.data = temp.data;
		temp = cacheSystemByteRead(cacheSystem, address + 1, ID);
		retVal.data = (retVal.data << 8) | temp.data;
		return retVal;
	}
	data = cacheSystemRead(cacheSystem, address, ID, 2);
	if (data == NULL) {
		retVal.success =false;
		return retVal;
	}
	retVal.data = data[0];
	retVal.data = (retVal.data << 8) | data[1];
	free(data);
	return retVal;
}


/*
	A function used to request a word from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
wordInfo_t cacheSystemWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	//halfWordInfo_t temp;
	wordInfo_t retVal;
	uint8_t* data ;
	/* Error Checking??*/
	retVal.success =true;
	if(address%4){
                retVal.success = false;
		return retVal;
     	}
	halfWordInfo_t temp;
	if (cacheSystem->blockDataSize < 4) {
		temp = cacheSystemHalfWordRead(cacheSystem, address, ID);
		retVal.data = temp.data;
		temp = cacheSystemHalfWordRead(cacheSystem, address + 2, ID);
		retVal.data = (retVal.data << 16) | temp.data;
		return retVal;
	}
	data = cacheSystemRead(cacheSystem, address, ID, 4);
	if (data == NULL) {
		retVal.success = false;
		return retVal;
	}
	retVal.data = data[0];
	retVal.data = (retVal.data << 8) | data[1];
	retVal.data = (retVal.data << 8) | data[2];
	retVal.data = (retVal.data << 8) | data[3];
	free(data);
	return retVal;
}

/*
	A function used to request a doubleword from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
doubleWordInfo_t cacheSystemDoubleWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID) {
	//wordInfo_t temp;
	doubleWordInfo_t retVal;
        uint8_t* data = cacheSystemRead(cacheSystem,address,ID,8);
        /* Error Checking??*/
	retVal.success =true;
        if(address%8){
                retVal.success = false;
		return retVal;
        }
	wordInfo_t temp;
	if (cacheSystem->blockDataSize < 8) {
		temp = cacheSystemWordRead(cacheSystem, address, ID);
		retVal.data = temp.data;
		temp = cacheSystemWordRead(cacheSystem, address + 4, ID);
		retVal.data = (retVal.data << 32) | temp.data;
		return retVal;
	}
	data = cacheSystemRead(cacheSystem, address, ID, 8);
	if (data == NULL) {
		retVal.success = false;
		return retVal;
	}
	retVal.data = data[0];
	retVal.data = (retVal.data << 8) | data[1];
	retVal.data = (retVal.data << 8) | data[2];
	retVal.data = (retVal.data << 8) | data[3];
	retVal.data = (retVal.data << 8) | data[4];
	retVal.data = (retVal.data << 8) | data[5];
	retVal.data = (retVal.data << 8) | data[6];
	retVal.data = (retVal.data << 8) | data[7];
	free(data);
	return retVal;
}
