/* Summer 2017 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "utils.h"
#include "cacheRead.h"
#include "mem.h"

/*
	Takes in a cache and a memeory address that is not located in the current
	cache and fetches it from main memory. 
*/
uint8_t* readFromMem(cache_t* cache, uint32_t address) {
	unsigned temp;
	FILE* memory = fopen(cache->physicalMemoryName, "r");
	uint8_t* data = malloc(sizeof(uint8_t) * cache->blockDataSize);
	if (data == NULL) {
		allocationFailed();
	}
	address = address - MIN_ADDRESS;
	fseek(memory, 3 * address, SEEK_SET);
	for (uint32_t i = 0; i < cache->blockDataSize; i++) {
		fscanf(memory, "%x", &temp);
		data[i] = (uint8_t) temp;
	}
	fclose(memory);
	return data;
}

/*
	Takes in a cache, a block number, and an address and writes the data in the
	block specified to phsyical memory at the address indicated.
*/
void writeToMem(cache_t* cache, uint32_t blockNumber, uint32_t address) {
	uint8_t* data = fetchBlock(cache, blockNumber);
	FILE* physicalMemory = fopen(cache->physicalMemoryName, "r+");
	address = address - MIN_ADDRESS;
	fseek(physicalMemory, 3 * address, SEEK_SET);
	for (int i = 0; i < cache->blockDataSize; i++) {
		if (data[i] < 16) {
			fprintf(physicalMemory, "0");
		}
		fprintf(physicalMemory, "%x ", data[i]);
	}
	fclose(physicalMemory);
	free(data);
}

/*
	Takes in an address and a size that will be requested and determines
	whether or not that memory is accessible. Returns 1 if the memory is
	accessible and 0 if it is not.
*/
int validAddresses(uint32_t address, uint32_t length) {
	if (address < MIN_ADDRESS || address > MAX_ADDRESS) {
		return 0;
	} else if (address + (length - 1) > MAX_ADDRESS) {
		return 0;
	}
	return 1;
}