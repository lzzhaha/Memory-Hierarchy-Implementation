/* Summer 2017 */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "setInCache.h"
#include "mem.h"
#include "cacheRead.h"
#include "cacheWrite.h"

int main() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	uint8_t* block;
	uint8_t* mem;
	wordInfo_t wordVal;


	//Read and write access pattern
	//Stolen from SP16_mt2
	n = 1;
	memFile = "testFiles/physicalMemory2.txt";
	blockDataSize = 8;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);

	uint32_t address = 0x61c00000;
	int i, j;
	int total = 0;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			writeWord(cache, address +  (((i << 3) + j) << 2), i + j);
		}
	}

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			wordVal = readWord(cache, address + (((i << 3) + j) << 2));
			total += (int) wordVal.data;
		}
	}

	mem = readFromMem(cache, address);
	block = fetchBlock(cache, 0);
	free(mem);
	free(block);

	deleteCache(cache);
	return 0;
}