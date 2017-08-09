/* Summer 2017 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "../part1/utils.h"
#include "../part1/mem.h"
#include "../part1/cacheRead.h"
#include "coherenceUtils.h"
#include "coherenceRead.h"
#include "coherenceWrite.h"

int main() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cacheSystem_t* sys;
	cacheNode_t** lst;
	byteInfo_t byteVal;
	uint32_t addr;
	uint8_t* mem;
	uint8_t* block;
	cache_t* cache;
	unsigned int j;

	addr = 0x61c10000;
	n = 1;
	blockDataSize = 8;
	totalDataSize = 1024;
	memFile = "testFiles/physicalMemory2.txt";
	lst = malloc(sizeof(cacheNode_t*) * 2);
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	lst[0] = createCacheNode(cache, 1);
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	lst[1] = createCacheNode(cache, 2);
	sys = createCacheSystem(lst, 2, createSnooper());
	//Create a two cache system
	for (unsigned int i = 0; i < 72; i++) {
		if (i & 1) {
			byteVal = cacheSystemByteRead(sys, addr + ((i >> 1) + 36), 2);
			cacheSystemByteWrite(sys, addr + ((i >> 1) + 36), 2, (uint8_t)  byteVal.data + 8);
		} else {
			byteVal = cacheSystemByteRead(sys, addr + (i >> 1), 1);
			cacheSystemByteWrite(sys, addr + (i >> 1), 1, (uint8_t)  byteVal.data + 8);
		}
	}
	for (j = 0; j < 5; j++) {
		cache = getCacheFromID(sys, 1);
		determineState(getCacheFromID(sys, 1), addr + (j << 3));
		determineState(getCacheFromID(sys, 2), addr + (j << 3));
		block = fetchBlock(cache, j);
		mem = readFromMem(cache, addr + (j << 3));
		free(block);
		free(mem);
	}
	for (;j < 9; j++) {
		cache = getCacheFromID(sys, 2);
		determineState(getCacheFromID(sys, 1), addr + (j << 3));
		determineState(getCacheFromID(sys, 2), addr + (j << 3));
		block = fetchBlock(cache, j);
		mem = readFromMem(cache, addr + (j << 3));
		free(block);
		free(mem);
	}
	deleteCacheSystem(sys);
	return 0;
}