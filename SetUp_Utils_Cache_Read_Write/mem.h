/* Summer 2017 */
#ifndef MEM_H
#define MEM_H
#define MIN_ADDRESS 0x61c00000
#define MAX_ADDRESS 0x61cfffff

/*
	Takes in a cache and a memeory address that is not located in the current
	cache and fetches it from main memory. 
*/
uint8_t* readFromMem(cache_t* cache, uint32_t address);

/*
	Takes in a cache, a block number, and an address and writes the data in the
	block specified to phsyical memory at the address indicated.
*/
void writeToMem(cache_t* cache, uint32_t blockNumber, uint32_t address);

/*
	Takes in an address and a size that will be requested and determines
	whether or not that memory is accessible. Returns 1 if the memory is
	accessible and 0 if it is not.
*/
int validAddresses(uint32_t address, uint32_t length);

#endif