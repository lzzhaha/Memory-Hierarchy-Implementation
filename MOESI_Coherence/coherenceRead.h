/* Summer 2017 */
#ifndef COHERENCEREAD_H
#define COHERENCEREAD_H

/*
	A function which processes all cache reads for an entire cache system.
	Takes in a cache system, an address, an id for a cache, and a size to read
	and calls the appropriate functions on the cache being selected to read
	the data. Returns the data if successful and otherwise NULL.
*/
uint8_t* cacheSystemRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID, uint8_t size);

/*
	A function used to request a byte from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
byteInfo_t cacheSystemByteRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID);

/*
	A function used to request a halfword from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
halfWordInfo_t cacheSystemHalfWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID);

/*
	A function used to request a word from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
wordInfo_t cacheSystemWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID);

/*
	A function used to request a doubleword from a specific cache in a cache system.
	Takes in a cache system, an address, and an ID for the cache which will be
	read from. Returns a struct with the data and a bool field indicating
	whether or not the read was a success.
*/
doubleWordInfo_t cacheSystemDoubleWordRead(cacheSystem_t* cacheSystem, uint32_t address, uint8_t ID);
#endif