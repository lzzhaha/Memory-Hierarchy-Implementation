/* Summer 2017 */
#include "coherenceUtils.h"
#include "coherenceWrite.h"
#include "../part1/mem.h"
#include "../part1/getFromCache.h"
#include "../part1/setInCache.h"
#include "../part1/cacheWrite.h"
#include "../part1/cacheRead.h"
#include "../part2/hitRate.h"

/*
	A function which processes all cache writes for an entire cache system.
	Takes in a cache system, an address, an id for a cache, an address, a size
	of data, and a pointer to data and calls the appropriate functions on the 
	cache being selected to write to the cache. 
*/
int cacheSystemWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t size, uint8_t* data) {
	 if(!validAddresses(address,size)||!cacheSystem||size<=0)
                {return -1;}
//	uint8_t* transferData;
	int result =0;
	evictionInfo_t* dstCacheInfo;
	evictionInfo_t* otherCacheInfo;
	uint32_t evictionBlockNumber;
	uint32_t offset;
	cacheNode_t** caches;
	//uint32_t tagVal;
	int otherCacheContains = 0;
	cache_t* dstCache = NULL;
	uint8_t counter = 0;
	caches = cacheSystem->caches;
	int otherCacheID = -1;
	while (dstCache == NULL && counter < cacheSystem->size) { //Selects destination cache pointer from array of caches pointers
		if (caches[counter]->ID == ID) {
			dstCache = caches[counter]->cache;
			offset = getOffset(dstCache,address);
		}
		counter++;
	}
	if(!dstCache){
		return -1;
	}
	dstCacheInfo = findEviction(dstCache, address); //Finds block to evict and potential match
	evictionBlockNumber = dstCacheInfo->blockNumber;
//	otherCacheContains = getShared(dstCache,evictionBlockNumber); 
	uint32_t tag = extractTag(dstCache,evictionBlockNumber);
	uint64_t dataLocation =getDataLocation(dstCache,evictionBlockNumber,offset); 
	uint64_t next_block_start = getBlockStartBits(dstCache,evictionBlockNumber+1);
	uint32_t index = extractIndex(dstCache,evictionBlockNumber);
	uint32_t new_tag = getTag(dstCache,address);


	if (dstCacheInfo->match) {
		/*What do you do if it is in the cache?*/
                 updateLRU(dstCache,tag,index,dstCacheInfo->LRU);
		 if ((size-1)*8+ dataLocation < next_block_start){//data fit into the block
                      setData(dstCache,data,evictionBlockNumber,size,offset);
		       setState(dstCache,evictionBlockNumber,MODIFIED);
		 }else{//data spread over multiple blocks
		      uint32_t segment_length = (next_block_start-dataLocation)/8;
		      setData(dstCache,data,evictionBlockNumber,segment_length,offset);
		      data+=segment_length;
		      size-=segment_length;
		      setTag(dstCache,new_tag,evictionBlockNumber);
		      updateLRU(dstCache,tag,index,dstCacheInfo->LRU);
		      if(cacheSystemWrite(cacheSystem,address+segment_length,ID,size,data)==-1){
				result = -1;
		      }
		}
		 setState(dstCache,evictionBlockNumber,MODIFIED);
		otherCacheContains = returnIDIf1(cacheSystem->snooper,address,cacheSystem->blockDataSize)==-1?1:0; 
	} else {
		uint32_t oldAddress = extractAddress(dstCache, extractTag(dstCache, evictionBlockNumber), evictionBlockNumber, 0);
		/*How do you need to update the snooper?*/
		/*How do you need to update states for what is getting evicted (don't worry about evicting this will be handled at a later step when you place data in the cache)?*/
		/*Your Code Here*/
		removeFromSnooper(cacheSystem->snooper,oldAddress,ID,cacheSystem->blockDataSize);
		int remaining_cache_ID = 0;
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

	
	
		otherCacheID = returnFirstCacheID(cacheSystem->snooper, address, cacheSystem->blockDataSize);
		/*Check other caches???*/
		/*Your Code Here*/
		otherCacheContains = (otherCacheID==-1)?0:1;
		
		if (!otherCacheContains) {
			/*Check Main memory?*/
			/*Your Code Here*/
			uint8_t* new_block = readFromMem(dstCache,address);
                        setData(dstCache,new_block,evictionBlockNumber,cacheSystem->blockDataSize,0);
                        free(new_block);
                        new_block = NULL;
             		//recursively write data
			setTag(dstCache,new_tag,evictionBlockNumber);
		        updateLRU(dstCache,new_tag,index,dstCacheInfo->LRU);
			setData(dstCache,data,evictionBlockNumber,size,offset);
			 setState(dstCache,evictionBlockNumber,MODIFIED);
		}

	}
	addToSnooper(cacheSystem->snooper, address, ID, cacheSystem->blockDataSize);
	if (otherCacheContains) {
		/*What states need to be updated?*/
		/*Does anything else need to be editted?*/
		/*Your Code Here*/
		if(!(dstCacheInfo->match)){
		
		cache_t* otherCache = getCacheFromID(cacheSystem,(uint8_t)otherCacheID);
		otherCacheInfo = findEviction(otherCache,address);
          	uint32_t otherCache_blockNumber = otherCacheInfo->blockNumber;
	        uint8_t* data_from_other_block = getData(otherCache,0,otherCache_blockNumber,cacheSystem->blockDataSize);
//load that block from other cache into the dstCache
        	writeWholeBlock(dstCache, address,evictionBlockNumber,data_from_other_block);
		setTag(dstCache,new_tag,evictionBlockNumber);
	        updateLRU(dstCache,new_tag,index,dstCacheInfo->LRU);
		setData(dstCache,data,evictionBlockNumber,size,offset);
		 setState(dstCache,evictionBlockNumber,MODIFIED);
		}
		for(uint32_t i=0; i< cacheSystem->size;i++){
                      	if(cacheSystem->caches[i]->ID!=ID){
				updateState(cacheSystem->caches[i]->cache,address,MODIFIED);
			}
                }

		/*
		//update the state of other cache
		while(true){
			//Invalid the block in the other cache
			setValid(otherCache,otherCache_blockNumber,0);
			uint32_t otherCache_tag = extractTag(otherCache,otherCache_blockNumber);
			uint32_t otherCache_index = extractIndex(otherCache,otherCache_blockNumber);
			decrementLRU(otherCache, otherCache_tag, otherCache_index, otherCacheInfo->LRU);
			
			//remove the address record in snooper
			removeFromSnooper(cacheSystem->snooper, address,(uint8_t)otherCacheID,cacheSystem->blockDataSize);
			otherCacheID =  returnFirstCacheID(cacheSystem->snooper, address, cacheSystem->blockDataSize);
			free(otherCacheInfo);
			if(otherCacheID==-1){//All other cache has been invalidated
				break;
			}else{
				otherCache = getCacheFromID(cacheSystem,(uint8_t)otherCacheID);
				otherCacheInfo = findEviction(otherCache,address);
				otherCache_blockNumber = otherCacheInfo->blockNumber;
			}
		}
		*/	
	}
	free(dstCacheInfo);
	return result;
}

/*
	A function used to write a byte to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemByteWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t data) {
	uint8_t array[1];
	array[0] = data;
	return cacheSystemWrite(cacheSystem, address, ID, 1, array);
	
}

/*
	A function used to write a halfword to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemHalfWordWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint16_t data) {
	if(address%2){
		return -1;
	
	}
	if (cacheSystem->blockDataSize < 2) {
		int result1 = cacheSystemByteWrite(cacheSystem, address, ID, (uint8_t) (data >> 8));
		int result2 = cacheSystemByteWrite(cacheSystem, address + 1, ID, (uint8_t) (data & UINT8_MAX));
		if(result1==-1 || result2 ==-1){
			return -1;
		}else{
			return 0;
		}
	}
	uint8_t array[2];
	array[0] = (uint8_t) (data >> 8);
	array[1] = (uint8_t) (data & UINT8_MAX);
	return cacheSystemWrite(cacheSystem, address, ID, 2, array);
}

/*
	A function used to write a word to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemWordWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint32_t data) {
	/* Error Checking??*/
	if(address%4){
		return -1;
	}
	if (cacheSystem->blockDataSize < 4) {
		int result1 = cacheSystemHalfWordWrite(cacheSystem, address, ID, (uint8_t) (data >> 16));
		int result2 = cacheSystemHalfWordWrite(cacheSystem, address + 2, ID, (uint8_t) (data & UINT16_MAX));
		if(result1 ==-1 || result2==-1){
			return -1;
		}else{
			return 0;
		}
	}
	uint8_t array[4];
	array[0] = (uint8_t) (data >> 24);
	array[1] = (uint8_t) ((data >> 16) & UINT8_MAX);
	array[2] = (uint8_t) ((data >> 8) & UINT8_MAX);
	array[3] = (uint8_t) (data & UINT8_MAX);
	return cacheSystemWrite(cacheSystem, address, ID, 4, array);
}

/*
	A function used to write a doubleword to a specific cache in a cache system.
	Takes in a cache system, an address, an ID, and data for the cache which
	will be written to. Returns 0 if the write is successful and otherwise
	returns -1.
*/
int cacheSystemDoubleWordWrite(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint64_t data) {
	/* Error Checking??*/
	if(address%8){
		return -1;
	}
	if (cacheSystem->blockDataSize < 8) {
		int result1 = cacheSystemWordWrite(cacheSystem, address, ID, (uint8_t) (data >> 32));
		int result2 = cacheSystemWordWrite(cacheSystem, address + 4, ID, (uint8_t) (data & UINT32_MAX));
		if(result1 ==-1 || result2 ==-1){
			return -1;
		}else{
			return 0;
		}
	}
	uint8_t array[8];
	array[0] = (uint8_t) (data >> 56);
	array[1] = (uint8_t) ((data >> 48) & UINT8_MAX);
	array[2] = (uint8_t) ((data >> 40) & UINT8_MAX);
	array[3] = (uint8_t) ((data >> 32) & UINT8_MAX);
	array[4] = (uint8_t) ((data >> 24) & UINT8_MAX);
	array[5] = (uint8_t) ((data >> 16) & UINT8_MAX);
	array[6] = (uint8_t) ((data >> 8) & UINT8_MAX);
	array[7] = (uint8_t) (data & UINT8_MAX);
	return 	cacheSystemWrite(cacheSystem, address, ID, 8, array);
}
