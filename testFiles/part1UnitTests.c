#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h> 
#include <CUnit/Basic.h>
#include "../part1/utils.h"
#include "../part1/getFromCache.h"
#include "../part1/setInCache.h"
#include "../part1/mem.h"
#include "../part1/cacheRead.h"
#include "../part1/cacheWrite.h"

void test_Utils() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/50AddressTest.txt";


	//Direct Mapped Cache Test
	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(cacheSizeBits(cache), 1248);
	CU_ASSERT_EQUAL(cacheSizeBytes(cache), 156);
	CU_ASSERT_EQUAL(numGarbageBits(cache), 0);
	CU_ASSERT_EQUAL(totalBlockBits(cache), 156);
	deleteCache(cache);

	//Fully Associative Cache Test
	n = 16;
	blockDataSize = 32;
	totalDataSize = 512;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(cacheSizeBits(cache), 4640);
	CU_ASSERT_EQUAL(cacheSizeBytes(cache), 580);
	CU_ASSERT_EQUAL(numGarbageBits(cache), 0);
	CU_ASSERT_EQUAL(totalBlockBits(cache), 290);
	deleteCache(cache);


	//Four Way Set Associtive Cache Test
	n = 4;
	blockDataSize = 32;
	totalDataSize = 256;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(cacheSizeBits(cache), 2296);
	CU_ASSERT_EQUAL(cacheSizeBytes(cache), 287);
	CU_ASSERT_EQUAL(numGarbageBits(cache), 0);
	CU_ASSERT_EQUAL(totalBlockBits(cache), 287);
	deleteCache(cache);

	//1 Block Test
	n = 1;
	blockDataSize = 2147483648;
	totalDataSize = 2147483648;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(cacheSizeBits(cache), 17179869188);
	CU_ASSERT_EQUAL(cacheSizeBytes(cache), 2147483649);
	CU_ASSERT_EQUAL(numGarbageBits(cache), 4);
	CU_ASSERT_EQUAL(totalBlockBits(cache), 17179869188);
	deleteCache(cache);

	//All Blocks Test
	n = 1;
	blockDataSize = 1;
	totalDataSize = 65536;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(cacheSizeBits(cache), 1769472);
	CU_ASSERT_EQUAL(cacheSizeBytes(cache), 221184);
	CU_ASSERT_EQUAL(numGarbageBits(cache), 0);
	CU_ASSERT_EQUAL(totalBlockBits(cache), 27);
	deleteCache(cache);

	//Smallest Cache Test
	n = 1;
	blockDataSize = 1;
	totalDataSize = 1;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(cacheSizeBits(cache), 43);
	CU_ASSERT_EQUAL(cacheSizeBytes(cache), 6);
	CU_ASSERT_EQUAL(numGarbageBits(cache), 5);
	CU_ASSERT_EQUAL(totalBlockBits(cache), 43);
	deleteCache(cache);
}

void test_Locations() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/50AddressTest.txt";

	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(getValidLocation(cache, 2), 312);
	CU_ASSERT_EQUAL(getDirtyLocation(cache, 1), 157);
	CU_ASSERT_EQUAL(getSharedLocation(cache, 3), 470);
	CU_ASSERT_EQUAL(getLRULocation(cache, 0), 3);
	CU_ASSERT_EQUAL(getTagLocation(cache, 0), 3);
	CU_ASSERT_EQUAL(getDataLocation(cache, 2, 9), 412);
	deleteCache(cache);


	n = 16;
	blockDataSize = 32;
	totalDataSize = 512;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(getValidLocation(cache, 6), 1740);
	CU_ASSERT_EQUAL(getDirtyLocation(cache, 11), 3191);
	CU_ASSERT_EQUAL(getSharedLocation(cache, 9), 2612);
	CU_ASSERT_EQUAL(getLRULocation(cache, 14), 4063);
	CU_ASSERT_EQUAL(getTagLocation(cache, 1), 297);
	CU_ASSERT_EQUAL(getDataLocation(cache, 15, 30), 4624);
	deleteCache(cache);

	n = 1;
	blockDataSize = 1;
	totalDataSize = 1;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(getValidLocation(cache, 0), 5);
	CU_ASSERT_EQUAL(getDirtyLocation(cache, 0), 6);
	CU_ASSERT_EQUAL(getSharedLocation(cache, 0), 7);
	CU_ASSERT_EQUAL(getLRULocation(cache, 0), 8);
	CU_ASSERT_EQUAL(getTagLocation(cache, 0), 8);
	CU_ASSERT_EQUAL(getDataLocation(cache, 0, 0), 40);
	deleteCache(cache);
}

void test_Bit() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/50AddressTest.txt";
	n = 1;
	blockDataSize = 1;
	totalDataSize = 8;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	cache->contents[0] = 0;
	setBit(cache, 6, 1);
	CU_ASSERT_EQUAL(cache->contents[0], 2);
	CU_ASSERT_EQUAL(getBit(cache, 6), 1);
	CU_ASSERT_EQUAL(getBit(cache, 7), 0);
	for (int i = 0; i < 6; i++) {
		CU_ASSERT_EQUAL(getBit(cache, i), 0);
	}

	cache->contents[4] = 0;
	setBit(cache, 36, 1);
	CU_ASSERT_EQUAL(cache->contents[4], 8);
	setBit(cache, 34, 1);
	CU_ASSERT_EQUAL(cache->contents[4], 40);
	for (int i = 0; i < 2; i++) {
		CU_ASSERT_EQUAL(getBit(cache, 32 + i), 0);
	}
	CU_ASSERT_EQUAL(getBit(cache, 34), 1);
	CU_ASSERT_EQUAL(getBit(cache, 35), 0);
	CU_ASSERT_EQUAL(getBit(cache, 36), 1);
	for (int i = 5; i < 8; i++) {
		CU_ASSERT_EQUAL(getBit(cache, 32 + i), 0);
	}

	cache->contents[1] = 255;
	setBit(cache, 13, 0);
	CU_ASSERT_EQUAL(cache->contents[1], 251);
	for (int i = 0; i < 5; i++) {
		CU_ASSERT_EQUAL(getBit(cache, 8 + i), 1);
	}
	CU_ASSERT_EQUAL(getBit(cache, 13), 0);
	for (int i = 6; i < 8; i++) {
		CU_ASSERT_EQUAL(getBit(cache, 8 + i), 1);
	}

	cache->contents[7] = 255;
	setBit(cache, 56, 0);
	CU_ASSERT_EQUAL(cache->contents[7], 127);
	setBit(cache, 63, 0);
	CU_ASSERT_EQUAL(cache->contents[7], 126);
	CU_ASSERT_EQUAL(getBit(cache, 56), 0);
	for (int i = 1; i < 7; i++) {
		CU_ASSERT_EQUAL(getBit(cache, 56 + i), 1);
	}
	CU_ASSERT_EQUAL(getBit(cache, 63), 0);

	deleteCache(cache);
}

void test_Getters() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/50AddressTest.txt";

	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	cache->contents[0] = 16;
	cache->contents[1] = 145;
	cache->contents[2] = 87;
	cache->contents[3] = 45;

	CU_ASSERT_EQUAL(getValid(cache, 0), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 0), 0);
	CU_ASSERT_EQUAL(getShared(cache, 0), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 0);
	CU_ASSERT_EQUAL(extractTag(cache, 0), 0x1091572);

	cache->contents[58] = 5;
	cache->contents[59] = 198;
	cache->contents[60] = 37;
	cache->contents[61] = 22;

	CU_ASSERT_EQUAL(getValid(cache, 3), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 3), 1);
	CU_ASSERT_EQUAL(getShared(cache, 3), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 0);
	CU_ASSERT_EQUAL(extractTag(cache, 3), 0x1c62516);

	deleteCache(cache);

	n = 16;
	blockDataSize = 32;
	totalDataSize = 512;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	cache->contents[36] = 63;
	cache->contents[37] = 89;
	cache->contents[38] = 209;
	cache->contents[39] = 97;
	cache->contents[40] = 0;

	CU_ASSERT_EQUAL(getValid(cache, 1), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 1), 1);
	CU_ASSERT_EQUAL(getShared(cache, 1), 1);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 14);
	CU_ASSERT_EQUAL(extractTag(cache, 1), 0x59d1610);

	deleteCache(cache);

}

void test_Getters_and_Setters() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	evictionInfo_t* evictInfo;
	uint8_t inputData[16];
	uint8_t* outputData;
	memFile = "testFiles/50AddressTest.txt";

	//Direct Mapped Cache
	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(getNumSets(cache), 8);
	CU_ASSERT_EQUAL(getTagSize(cache), 25);

	setValid(cache, 1, 1);
	setValid(cache, 3, 1);
	setValid(cache, 6, 1);
	setDirty(cache, 2, 1);
	setDirty(cache, 0, 1);
	setDirty(cache, 6, 1);
	setShared(cache, 3, 1);
	setShared(cache, 6, 1);
	setShared(cache, 2, 1);
	setShared(cache, 7, 1);
	CU_ASSERT_EQUAL(getValid(cache, 0), 0);
	CU_ASSERT_EQUAL(getValid(cache, 1), 1);
	CU_ASSERT_EQUAL(getValid(cache, 2), 0);
	CU_ASSERT_EQUAL(getValid(cache, 3), 1);
	CU_ASSERT_EQUAL(getValid(cache, 4), 0);
	CU_ASSERT_EQUAL(getValid(cache, 5), 0);
	CU_ASSERT_EQUAL(getValid(cache, 6), 1);
	CU_ASSERT_EQUAL(getValid(cache, 7), 0);
	CU_ASSERT_EQUAL(getShared(cache, 3), 1);
	CU_ASSERT_EQUAL(getShared(cache, 6), 1);
	CU_ASSERT_EQUAL(getShared(cache, 2), 1);
	CU_ASSERT_EQUAL(getShared(cache, 7), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 2), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 0), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 6), 1);
	setValid(cache, 3, 0);
	setDirty(cache, 0, 0);
	setDirty(cache, 1, 0);
	setDirty(cache, 3, 0);
	setDirty(cache, 4, 0);
	setDirty(cache, 5, 0);
	setDirty(cache, 7, 0);
	setShared(cache, 0, 0);
	setShared(cache, 1, 0);
	setShared(cache, 4, 0);
	setShared(cache, 5, 0);
	setShared(cache, 7, 0);
	CU_ASSERT_EQUAL(getValid(cache, 0), 0);
	CU_ASSERT_EQUAL(getValid(cache, 1), 1);
	CU_ASSERT_EQUAL(getValid(cache, 2), 0);
	CU_ASSERT_EQUAL(getValid(cache, 3), 0);
	CU_ASSERT_EQUAL(getValid(cache, 4), 0);
	CU_ASSERT_EQUAL(getValid(cache, 5), 0);
	CU_ASSERT_EQUAL(getValid(cache, 6), 1);
	CU_ASSERT_EQUAL(getValid(cache, 7), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 0), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 1), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 2), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 3), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 4), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 5), 0);
	CU_ASSERT_EQUAL(getDirty(cache, 6), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 7), 0);
	CU_ASSERT_EQUAL(getShared(cache, 0), 0);
	CU_ASSERT_EQUAL(getShared(cache, 1), 0);
	CU_ASSERT_EQUAL(getShared(cache, 2), 1);
	CU_ASSERT_EQUAL(getShared(cache, 3), 1);
	CU_ASSERT_EQUAL(getShared(cache, 4), 0);
	CU_ASSERT_EQUAL(getShared(cache, 5), 0);
	CU_ASSERT_EQUAL(getShared(cache, 6), 1);
	CU_ASSERT_EQUAL(getShared(cache, 7), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 4), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 5), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 6), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 7), 0);
	CU_ASSERT_EQUAL(getTag(cache, 0x61c00021), 0x00c38000);
	CU_ASSERT_EQUAL(getIndex(cache, 0x61c00021), 2);
	CU_ASSERT_EQUAL(getOffset(cache, 0x61c00021), 1);
	setTag(cache, getTag(cache, 0x61c00021), 2);
	CU_ASSERT_EQUAL(extractTag(cache, 2), 0x00c38000);
	CU_ASSERT_EQUAL(extractIndex(cache, 2), 2);
	CU_ASSERT_EQUAL(extractAddress(cache, extractTag(cache, 2), 2, 1), 0x61c00021);
	CU_ASSERT_EQUAL(getLRUAddress(cache, 0x61c00021), 0);

	inputData[0] = 0x1e;
	inputData[1] = 0x4;
	inputData[2] = 0x90;
	inputData[3] = 0x66;
	inputData[4] = 0xb3;
	inputData[5] = 0xc6;
	inputData[6] = 0x18;
	inputData[7] = 0x9f;
	inputData[8] = 0xe3;
	inputData[9] = 0x5a;
	inputData[10] = 0xa3;
	inputData[11] = 0x14;
	inputData[12] = 0x24;
	inputData[13] = 0x32;
	inputData[14] = 0x25;
	inputData[15] = 0xa4;
	setData(cache, inputData, 2, 16, 0);
	outputData = getData(cache, getOffset(cache, 0x61c00020), 2, 16);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(inputData[i], outputData[i]);
	}
	free(outputData);
	inputData[7] = 0x67;
	setData(cache, inputData + 7, 2, 1, 7);
	outputData = getData(cache, getOffset(cache, 0x61c00020), 2, 16);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(inputData[i], outputData[i]);
	}
	free(outputData);
	outputData = getData(cache, getOffset(cache, 0x61c00027), 2, 1);
	CU_ASSERT_EQUAL(inputData[7], outputData[0]);
	free(outputData);

	clearCache(cache);
	CU_ASSERT_EQUAL(getValid(cache, 0), 0);
	CU_ASSERT_EQUAL(getValid(cache, 1), 0);
	CU_ASSERT_EQUAL(getValid(cache, 2), 0);
	CU_ASSERT_EQUAL(getValid(cache, 3), 0);
	CU_ASSERT_EQUAL(getValid(cache, 4), 0);
	CU_ASSERT_EQUAL(getValid(cache, 5), 0);
	CU_ASSERT_EQUAL(getValid(cache, 6), 0);
	CU_ASSERT_EQUAL(getValid(cache, 7), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 4), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 5), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 6), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 7), 0);
	deleteCache(cache);


	//Fully Associative Cache
	n = 4;
	blockDataSize = 16;
	totalDataSize = 64;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	CU_ASSERT_EQUAL(getNumSets(cache), 1);
	CU_ASSERT_EQUAL(getTagSize(cache), 28);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 3);
	setLRU(cache, 3, 1);
	setTag(cache, getTag(cache, 0x61cffff0), 0);
	CU_ASSERT_EQUAL(extractTag(cache, 0), 0x61cffff);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 1);
	updateLRU(cache, getTag(cache, 0x61cffff0), 0, 2);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 2);
	setValid(cache, 0, 1);




	setData(cache, inputData, 0, 16, 0);
	outputData = getData(cache, getOffset(cache, 0x61cffff0), 0, 16);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(inputData[i], outputData[i]);
	}
	free(outputData);
	inputData[9] = 0x2e;
	setData(cache, inputData + 9, 0, 1, 9);
	outputData = getData(cache, getOffset(cache, 0x61cffff0), 0, 16);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(inputData[i], outputData[i]);
	}
	free(outputData);
	outputData = getData(cache, getOffset(cache, 0x61cffff9), 0, 1);
	CU_ASSERT_EQUAL(inputData[9], outputData[0]);
	free(outputData);



	updateLRU(cache, getTag(cache, 0x61cffff0), 0, 3);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 0);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 3);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 3);
	setLRU(cache, 1, 1);
	setLRU(cache, 2, 2);
	updateLRU(cache, 0, 0, 1);
	CU_ASSERT_EQUAL(getLRU(cache, 0), 1);
	CU_ASSERT_EQUAL(getLRU(cache, 1), 1);
	CU_ASSERT_EQUAL(getLRU(cache, 2), 2);
	CU_ASSERT_EQUAL(getLRU(cache, 3), 3);
	evictInfo = findEviction(cache, 0);
	CU_ASSERT_EQUAL(evictInfo->blockNumber, 3);
	CU_ASSERT_EQUAL(evictInfo->match, false);
	CU_ASSERT_EQUAL(evictInfo->LRU, 3);
	free(evictInfo);
	evictInfo = findEviction(cache, 0x61cffff0);
	CU_ASSERT_EQUAL(evictInfo->blockNumber, 0);
	CU_ASSERT_EQUAL(evictInfo->match, true);
	CU_ASSERT_EQUAL(evictInfo->LRU, 1);
	free(evictInfo);
	setValid(cache, 0, 0);
	evictInfo = findEviction(cache, 0x61cffff0);
	CU_ASSERT_EQUAL(evictInfo->blockNumber, 3);
	CU_ASSERT_EQUAL(evictInfo->match, false);
	CU_ASSERT_EQUAL(evictInfo->LRU, 3);
	free(evictInfo);
	deleteCache(cache);
}

void test_Extraction() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/physicalMemory1.txt";

	//Small Direct Mapped Cache
	n = 1;
	blockDataSize = 8;
	totalDataSize = 64;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(extractIndex(cache, i), i);
	}

	setTag(cache, getTag(cache, 0x61cf0000), 0);
	setTag(cache, getTag(cache, 0x61cf2017), 2);
	setTag(cache, getTag(cache, 0x61c92122), 4);
	setTag(cache, getTag(cache, 0x61c980cf), 1);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf0000), 0, 0), 0x61cf0000);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf2017), 2, 7), 0x61cf2017);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c92122), 4, 2), 0x61c92122);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c980cf), 1, 7), 0x61c980cf);
	deleteCache(cache);

	//Big Direct Mapped Cache
	n = 1;
	blockDataSize = 32;
	totalDataSize = 16384;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 512; i++) {
		CU_ASSERT_EQUAL(extractIndex(cache, i), i);
	}

	setTag(cache, getTag(cache, 0x61cf0000), 0);
	setTag(cache, getTag(cache, 0x61cf2017), 256);
	setTag(cache, getTag(cache, 0x61c92122), 265);
	setTag(cache, getTag(cache, 0x61c980cf), 6);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf0000), 0, 0), 0x61cf0000);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf2017), 256, 23), 0x61cf2017);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c92122), 265, 2), 0x61c92122);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c980cf), 6, 15), 0x61c980cf);
	deleteCache(cache);
	//Small Fully Associative Cache
	n = 8;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(extractIndex(cache, i), 0);
	}
  printCache(cache);

	setTag(cache, getTag(cache, 0x61cf0000), 0);
	setTag(cache, getTag(cache, 0x61cf2017), 3);
	setTag(cache, getTag(cache, 0x61c92122), 5);
	setTag(cache, getTag(cache, 0x61c980cf), 7);
  printCache(cache);
  
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf0000), 0, 0), 0x61cf0000);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf2017), 3, 7), 0x61cf2017);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c92122), 5, 2), 0x61c92122);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c980cf), 7, 15), 0x61c980cf);
	deleteCache(cache);

	//Big Fully Associative Cache
	n = 256;
	blockDataSize = 128;
	totalDataSize = 32768;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 256; i++) {
		CU_ASSERT_EQUAL(extractIndex(cache, i), 0);
	}

	setTag(cache, getTag(cache, 0x61cf0000), 0);
	setTag(cache, getTag(cache, 0x61cf2017), 35);
	setTag(cache, getTag(cache, 0x61c92122), 234);
	setTag(cache, getTag(cache, 0x61c980cf), 187);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf0000), 0, 0), 0x61cf0000);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf2017), 35, 23), 0x61cf2017);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c92122), 234, 34), 0x61c92122);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c980cf), 187, 79), 0x61c980cf);
	deleteCache(cache);

	//Small N-ways Set Associative Cache
	n = 4;
	blockDataSize = 4;
	totalDataSize = 32;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(extractIndex(cache, i), (i >> 2));
	}

	setTag(cache, getTag(cache, 0x61cf0000), 0);
	setTag(cache, getTag(cache, 0x61cf2017), 6);
	setTag(cache, getTag(cache, 0x61c92122), 3);
	setTag(cache, getTag(cache, 0x61c980cf), 5);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf0000), 0, 0), 0x61cf0000);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf2017), 6, 3), 0x61cf2017);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c92122), 3, 2), 0x61c92122);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c980cf), 5, 3), 0x61c980cf);
	deleteCache(cache);

	//Big N-ways Set Associative Cache
	n = 128;
	blockDataSize = 2;
	totalDataSize = 8192;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 4096; i++) {
		CU_ASSERT_EQUAL(extractIndex(cache, i), (i >> 7));
	}
	
	setTag(cache, getTag(cache, 0x61cf0000), 0);
	setTag(cache, getTag(cache, 0x61cf2017), 1471);
	setTag(cache, getTag(cache, 0x61c92122), 2200);
	setTag(cache, getTag(cache, 0x61c980cf), 1001);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf0000), 0, 0), 0x61cf0000);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61cf2017), 1471, 1), 0x61cf2017);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c92122), 2200, 0), 0x61c92122);
	CU_ASSERT_EQUAL(extractAddress(cache, getTag(cache, 0x61c980cf), 1001, 1), 0x61c980cf);
	deleteCache(cache);

}

void test_Mem() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	uint8_t* outputData;
	uint8_t fileContents_1[8];
	uint8_t fileContents_2[16];
	memFile = "testFiles/100AddressTest.txt";
	
	//Tests that only valid addresses are accepted.
	CU_ASSERT_EQUAL(validAddresses(0x61cffff0, 1), 1);
	CU_ASSERT_EQUAL(validAddresses(0x61c00021, 900), 1);
	CU_ASSERT_EQUAL(validAddresses(0x3253, 42), 0);
	CU_ASSERT_EQUAL(validAddresses(0x62c93843, 50), 0);
	CU_ASSERT_EQUAL(validAddresses(0x61c00000, 1), 1);
	CU_ASSERT_EQUAL(validAddresses(0x61cffff9, 7), 1);
	CU_ASSERT_EQUAL(validAddresses(0x61bfffff, 2), 0);
	CU_ASSERT_EQUAL(validAddresses(0x61cffff9, 8), 0);


	//Test Reading from main memory
	n = 1;
	blockDataSize = 8;
	totalDataSize = 16;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	outputData = readFromMem(cache, 0x61c00000);
	fileContents_1[0] = 0x12;
	fileContents_1[1] = 0x45;
	fileContents_1[2] = 0xdd;
	fileContents_1[3] = 0x0c;
	fileContents_1[4] = 0x8f;
	fileContents_1[5] = 0x14;
	fileContents_1[6] = 0x99;
	fileContents_1[7] = 0x96;
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_1[i]);
	}
	free(outputData);
	outputData = readFromMem(cache, 0x61c00058);
	fileContents_1[0] = 0x86;
	fileContents_1[1] = 0x39;
	fileContents_1[2] = 0xde;
	fileContents_1[3] = 0xb8;
	fileContents_1[4] = 0x0c;
	fileContents_1[5] = 0x21;
	fileContents_1[6] = 0x99;
	fileContents_1[7] = 0xd9;
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_1[i]);
	}
	free(outputData);

	//Test Writing to main memory
	setData(cache, fileContents_1, 0, 8, 0);
	writeToMem(cache, 0, 0x61c00010);
	outputData = readFromMem(cache, 0x61c00010);
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_1[i]);
	}
	free(outputData);
	writeToMem(cache, 0, 0x61c00000);
	outputData = readFromMem(cache, 0x61c00000);
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_1[i]);
	}
	free(outputData);
	deleteCache(cache);

	n = 2;
	blockDataSize = 16;
	totalDataSize = 64;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	//Test Reading from main memory with a larger block size
	outputData = readFromMem(cache, 0x61c00040);
	fileContents_2[0] = 0xa3;
	fileContents_2[1] = 0x79;
	fileContents_2[2] = 0x4c;
	fileContents_2[3] = 0xfd;
	fileContents_2[4] = 0x86;
	fileContents_2[5] = 0x7b;
	fileContents_2[6] = 0x2f;
	fileContents_2[7] = 0xd3;
	fileContents_2[8] = 0xbc;
	fileContents_2[9] = 0x6f;
	fileContents_2[10] = 0x16;
	fileContents_2[11] = 0xb1;
	fileContents_2[12] = 0xb2;
	fileContents_2[13] = 0x19;
	fileContents_2[14] = 0x1c;
	fileContents_2[15] = 0x7c;
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_2[i]);
	}
	free(outputData);
	outputData = readFromMem(cache, 0x61c00050);
	fileContents_2[0] = 0x0f;
	fileContents_2[1] = 0x54;
	fileContents_2[2] = 0xb4;
	fileContents_2[3] = 0x23;
	fileContents_2[4] = 0xb9;
	fileContents_2[5] = 0xfd;
	fileContents_2[6] = 0x8c;
	fileContents_2[7] = 0x03;
	fileContents_2[8] = 0x86;
	fileContents_2[9] = 0x39;
	fileContents_2[10] = 0xde;
	fileContents_2[11] = 0xb8;
	fileContents_2[12] = 0x0c;
	fileContents_2[13] = 0x21;
	fileContents_2[14] = 0x99;
	fileContents_2[15] = 0xd9;
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_2[i]);
	}
	free(outputData);

	//Test Writing to main memory with a larger block size
	fileContents_2[7] = 0x89;
	fileContents_2[1] = 0x00;
	setData(cache, fileContents_2, 2, 16, 0);
	writeToMem(cache, 2, 0x61c00020);
	outputData = readFromMem(cache, 0x61c00020);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_2[i]);
	}
	free(outputData);
	writeToMem(cache, 2, 0x61c00000);
	outputData = readFromMem(cache, 0x61c00000);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(outputData[i], fileContents_2[i]);
	}
	free(outputData);
	deleteCache(cache);
}

void test_Read() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	uint32_t wordVal;
	uint64_t doubleVal;
	char* memFile;
	cache_t* cache;
	uint8_t input1[16];
	uint8_t input2[16];
	uint8_t input3[16];
	uint8_t input4[16];
	uint8_t* outputData;
	uint8_t* secondOutput;
	byteInfo_t byteRead;
	halfWordInfo_t halfWordRead;
	wordInfo_t wordRead;
	doubleWordInfo_t doubleRead;
	memFile = "testFiles/physicalMemory1.txt";


	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	//Make basic reads in cache


	input1[0] = 0xa3;
	input1[1] = 0x79;
	input1[2] = 0x4c;
	input1[3] = 0xfd;
	input1[4] = 0x86;
	input1[5] = 0x7b;
	input1[6] = 0x2f;
	input1[7] = 0xd3;
	input1[8] = 0xbc;
	input1[9] = 0x6f;
	input1[10] = 0x16;
	input1[11] = 0xb1;
	input1[12] = 0xb2;
	input1[13] = 0x19;
	input1[14] = 0x1c;
	input1[15] = 0x7c;
	setValid(cache, 1, 1);
	setDirty(cache, 1, 0);
	setTag(cache, getTag(cache, 0x61c44410), 1);
	setData(cache, input1, 1, 16, 0);


	input2[0] = 0xa3;
	input2[1] = 0x79;
	input2[2] = 0x4c;
	input2[3] = 0xf3;
	input2[4] = 0x81;
	input2[5] = 0x7b;
	input2[6] = 0x2f;
	input2[7] = 0x43;
	input2[8] = 0xbc;
	input2[9] = 0x6f;
	input2[10] = 0x16;
	input2[11] = 0xb1;
	input2[12] = 0xe2;
	input2[13] = 0x19;
	input2[14] = 0x8c;
	input2[15] = 0x70;
	setValid(cache, 2, 1);
	setDirty(cache, 2, 0);
	setTag(cache, getTag(cache, 0x61c00020), 2);
	setData(cache, input2, 2, 16, 0);

	input3[0] = 0x03;
	input3[1] = 0x79;
	input3[2] = 0x4c;
	input3[3] = 0xf3;
	input3[4] = 0x81;
	input3[5] = 0x7b;
	input3[6] = 0x2f;
	input3[7] = 0x00;
	input3[8] = 0xbc;
	input3[9] = 0x64;
	input3[10] = 0x16;
	input3[11] = 0xb1;
	input3[12] = 0x02;
	input3[13] = 0x19;
	input3[14] = 0x8c;
	input3[15] = 0x70;
	setValid(cache, 4, 1);
	setDirty(cache, 4, 0);
	setTag(cache, getTag(cache, 0x61c66640), 4);
	setData(cache, input3, 4, 16, 0);

	input4[0] = 0xd3;
	input4[1] = 0x79;
	input4[2] = 0x4c;
	input4[3] = 0xf3;
	input4[4] = 0x8e;
	input4[5] = 0x7b;
	input4[6] = 0x2f;
	input4[7] = 0x00;
	input4[8] = 0xdd;
	input4[9] = 0x64;
	input4[10] = 0x16;
	input4[11] = 0xbb;
	input4[12] = 0x02;
	input4[13] = 0x19;
	input4[14] = 0x0c;
	input4[15] = 0x7e;
	setValid(cache, 7, 1);
	setDirty(cache, 7, 0);
	setTag(cache, getTag(cache, 0x61cbba70), 7);
	setData(cache, input4, 7, 16, 0);

	outputData = fetchBlock(cache, 1);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input1[i], outputData[i]);
	}
	free(outputData);

	outputData = fetchBlock(cache, 2);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input2[i], outputData[i]);
	}
	free(outputData);

	outputData = fetchBlock(cache, 4);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input3[i], outputData[i]);
	}
	free(outputData);

	outputData = fetchBlock(cache, 7);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input4[i], outputData[i]);
	}
	free(outputData);

	byteRead = readByte(cache, 0x61c66643);
	CU_ASSERT_EQUAL(byteRead.success, true);
	CU_ASSERT_EQUAL(byteRead.data, input3[3]);
	
	//printCache(cache);

	byteRead = readByte(cache, 0x61c0002c);
	CU_ASSERT_EQUAL(byteRead.success, true);
	CU_ASSERT_EQUAL(byteRead.data, input2[12]);

//	printCache(cache);

	
	byteRead = readByte(cache, 0x61cbba70);
	CU_ASSERT_EQUAL(byteRead.success, true);
	CU_ASSERT_EQUAL(byteRead.data, input4[0]);

//	printCache(cache);

	byteRead = readByte(cache, 0x61c4441f);
	CU_ASSERT_EQUAL(byteRead.success, true);
	CU_ASSERT_EQUAL(byteRead.data, input1[15]);

	//printCache(cache);

	halfWordRead = readHalfWord(cache, 0x61c4441a);
	CU_ASSERT_EQUAL(halfWordRead.success, true);
	CU_ASSERT_EQUAL(halfWordRead.data, (((uint16_t) input1[10]) << 8) | input1[11]);

//	printCache(cache);

	halfWordRead = readHalfWord(cache, 0x61cbba76);
	CU_ASSERT_EQUAL(halfWordRead.success, true);
	CU_ASSERT_EQUAL(halfWordRead.data, (((uint16_t) input4[6]) << 8) | input4[7]);

//	printCache(cache);

	halfWordRead = readHalfWord(cache, 0x61c00020);
	CU_ASSERT_EQUAL(halfWordRead.success, true);
	CU_ASSERT_EQUAL(halfWordRead.data, (((uint16_t) input2[0]) << 8) | input2[1]);

	halfWordRead = readHalfWord(cache, 0x61c6664e);
	CU_ASSERT_EQUAL(halfWordRead.success, true);
	CU_ASSERT_EQUAL(halfWordRead.data, (((uint16_t) input3[14]) << 8) | input3[15]);

//	printCache(cache);
	
	wordRead = readWord(cache, 0x61c44410);
	CU_ASSERT_EQUAL(wordRead.success, true);
	wordVal = (((uint32_t) input1[0]) << 24) | (((uint32_t) input1[1]) << 16) | (((uint32_t) input1[2]) << 8) | input1[3];
	CU_ASSERT_EQUAL(wordRead.data, wordVal);
	
//	printCache(cache);


	wordRead = readWord(cache, 0x61c00024);
	CU_ASSERT_EQUAL(wordRead.success, true);
	wordVal = (((uint32_t) input2[4]) << 24) | (((uint32_t) input2[5]) << 16) | (((uint32_t) input2[6]) << 8) | input2[7];
	CU_ASSERT_EQUAL(wordRead.data, wordVal);

	printCache(cache);

	wordRead = readWord(cache, 0x61c66648);
	CU_ASSERT_EQUAL(wordRead.success, true);
	wordVal = (((uint32_t) input3[8]) << 24) | (((uint32_t) input3[9]) << 16) | (((uint32_t) input3[10]) << 8) | input3[11];
	CU_ASSERT_EQUAL(wordRead.data, wordVal);

	printCache(cache);


	wordRead = readWord(cache, 0x61cbba7c);	
	CU_ASSERT_EQUAL(wordRead.success, true);
	wordVal = (((uint32_t) input4[12]) << 24) | (((uint32_t) input4[13]) << 16) | (((uint32_t) input4[14]) << 8) | input4[15];
	CU_ASSERT_EQUAL(wordRead.data, wordVal);
	
	printCache(cache);	

	doubleRead = readDoubleWord(cache, 0x61c44418);
	CU_ASSERT_EQUAL(doubleRead.success, true);
	doubleVal = (((uint64_t) input1[8]) << 56) | (((uint64_t) input1[9]) << 48) | (((uint64_t) input1[10]) << 40) | (((uint64_t) input1[11]) << 32)
	| (((uint64_t) input1[12]) << 24) | (((uint64_t) input1[13]) << 16) | (((uint64_t) input1[14]) << 8) | input1[15];
	CU_ASSERT_EQUAL(doubleRead.data, doubleVal);
	
	printCache(cache);

	doubleRead = readDoubleWord(cache, 0x61c00020);
	CU_ASSERT_EQUAL(doubleRead.success, true);
	doubleVal = (((uint64_t) input2[0]) << 56) | (((uint64_t) input2[1]) << 48) | (((uint64_t) input2[2]) << 40) | (((uint64_t) input2[3]) << 32)
	| (((uint64_t) input2[4]) << 24) | (((uint64_t) input2[5]) << 16) | (((uint64_t) input2[6]) << 8) | input2[7];
	CU_ASSERT_EQUAL(doubleRead.data, doubleVal);

	doubleRead = readDoubleWord(cache, 0x61c66640);
	CU_ASSERT_EQUAL(doubleRead.success, true);
	doubleVal = (((uint64_t) input3[0]) << 56) | (((uint64_t) input3[1]) << 48) | (((uint64_t) input3[2]) << 40) | (((uint64_t) input3[3]) << 32)
	| (((uint64_t) input3[4]) << 24) | (((uint64_t) input3[5]) << 16) | (((uint64_t) input3[6]) << 8) | input3[7];
	CU_ASSERT_EQUAL(doubleRead.data, doubleVal);

	doubleRead = readDoubleWord(cache, 0x61cbba78);
	CU_ASSERT_EQUAL(doubleRead.success, true);
	doubleVal = (((uint64_t) input4[8]) << 56) | (((uint64_t) input4[9]) << 48) | (((uint64_t) input4[10]) << 40) | (((uint64_t) input4[11]) << 32)
	| (((uint64_t) input4[12]) << 24) | (((uint64_t) input4[13]) << 16) | (((uint64_t) input4[14]) << 8) | input4[15];
	CU_ASSERT_EQUAL(doubleRead.data, doubleVal);
	
	printCache(cache);
	
	//Make reads that are not in cache
	byteRead = readByte(cache, 0x61cddbd3);
	printCache(cache);
	outputData = fetchBlock(cache, 5);
	secondOutput = readFromMem(cache, 0x61cddbd0);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(outputData[i], secondOutput[i]);
	}
	CU_ASSERT_EQUAL(byteRead.success, true);
	CU_ASSERT_EQUAL(byteRead.data, secondOutput[3]);
	free(outputData);
	free(secondOutput);

	halfWordRead = readHalfWord(cache, 0x61cff0f6);
	outputData = fetchBlock(cache, 7);
	secondOutput = readFromMem(cache, 0x61cff0f0);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(outputData[i], secondOutput[i]);
	}
	CU_ASSERT_EQUAL(halfWordRead.success, true);
	CU_ASSERT_EQUAL(halfWordRead.data, (((uint16_t) secondOutput[6]) << 8) | secondOutput[7]);
	free(outputData);
	free(secondOutput);

	wordRead = readWord(cache, 0x61c61c34);
	outputData = fetchBlock(cache, 3);
	secondOutput = readFromMem(cache, 0x61c61c30);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(outputData[i], secondOutput[i]);
	}
	CU_ASSERT_EQUAL(wordRead.success, true);
	wordVal = ((uint32_t) secondOutput[4] << 24) | ((uint32_t) secondOutput[5] << 16) | ((uint32_t) secondOutput[6] << 8) | secondOutput[7];
	CU_ASSERT_EQUAL(wordRead.data, wordVal);
	free(outputData);
	free(secondOutput);

	doubleRead = readDoubleWord(cache, 0x61c61b00);
	outputData = fetchBlock(cache, 0);
	secondOutput = readFromMem(cache, 0x61c61b00);
	for (uint8_t i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(outputData[i], secondOutput[i]);
	}
	CU_ASSERT_EQUAL(doubleRead.success, true);
	doubleVal = ((uint64_t) secondOutput[0] << 56) | ((uint64_t) secondOutput[1] << 48) | ((uint64_t) secondOutput[2] << 40) | ((uint64_t) secondOutput[3] << 32)
	| ((uint64_t) secondOutput[4] << 24) | ((uint64_t) secondOutput[5] << 16) | ((uint64_t) secondOutput[6] << 8) | secondOutput[7];
	CU_ASSERT_EQUAL(doubleRead.data, doubleVal);
	free(outputData);
	free(secondOutput);


	deleteCache(cache);

}

void test_ReadErrors() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	byteInfo_t byteRead;
	halfWordInfo_t halfWordRead;
	wordInfo_t wordRead;
	doubleWordInfo_t doubleRead;
	memFile = "testFiles/physicalMemory1.txt";


	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);


	//Verify address restriction errors
	byteRead = readByte(cache, 0x61bfffff);
	CU_ASSERT_EQUAL(byteRead.success, false);
	byteRead = readByte(cache, 0x61d00000);
	CU_ASSERT_EQUAL(byteRead.success, false);

	halfWordRead = readHalfWord(cache, 0x61bffffe);
	CU_ASSERT_EQUAL(halfWordRead.success, false);
	halfWordRead = readHalfWord(cache, 0x61d00000);
	CU_ASSERT_EQUAL(halfWordRead.success, false);

	wordRead = readWord(cache, 0x61bffffc);
	CU_ASSERT_EQUAL(wordRead.success, false);
	wordRead = readWord(cache, 0x61d00000);
	CU_ASSERT_EQUAL(wordRead.success, false);

	doubleRead = readDoubleWord(cache, 0x61bffff8);
	CU_ASSERT_EQUAL(doubleRead.success, false);
	doubleRead = readDoubleWord(cache, 0x61d00000);
	CU_ASSERT_EQUAL(doubleRead.success, false);

}

void test_Write() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	uint8_t* block;
	uint8_t* mem;
	uint8_t input1[16];
	uint8_t input2[16];
	uint8_t input3[16];
	uint8_t input4[16];
	memFile = "testFiles/physicalMemory1.txt";



	//Verify allignment restriction errors
	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	//Make basic writes in cache
	input1[0] = 0x13;
	input1[1] = 0x71;
	input1[2] = 0x30;
	input1[3] = 0x00;
	input1[4] = 0xbb;
	input1[5] = 0xe6;
	input1[6] = 0xd;
	input1[7] = 0xf1;
	input1[8] = 0xa4;
	input1[9] = 0xc0;
	input1[10] = 0x32;
	input1[11] = 0x88;
	input1[12] = 0x2b;
	input1[13] = 0xa3;
	input1[14] = 0x27;
	input1[15] = 0x80;
	setValid(cache, 0, 1);
	setTag(cache, getTag(cache, 0x61c00000), 0);
	setData(cache, input1, 0, 16, 0);



	input2[0] = 0x13;
	input2[1] = 0x74;
	input2[2] = 0x3b;
	input2[3] = 0x00;
	input2[4] = 0xbb;
	input2[5] = 0xe6;
	input2[6] = 0xda;
	input2[7] = 0xf1;
	input2[8] = 0xa4;
	input2[9] = 0xc0;
	input2[10] = 0xe2;
	input2[11] = 0x88;
	input2[12] = 0x2b;
	input2[13] = 0xf3;
	input2[14] = 0x27;
	input2[15] = 0x80;
	setValid(cache, 3, 1);
	setTag(cache, getTag(cache, 0x61c11134), 3);
	setData(cache, input2, 3, 16, 0);


	input3[0] = 0x53;
	input3[1] = 0x74;
	input3[2] = 0x3b;
	input3[3] = 0x00;
	input3[4] = 0xb9;
	input3[5] = 0xe6;
	input3[6] = 0xda;
	input3[7] = 0xf1;
	input3[8] = 0xa4;
	input3[9] = 0xc2;
	input3[10] = 0xe2;
	input3[11] = 0x88;
	input3[12] = 0x2b;
	input3[13] = 0xf3;
	input3[14] = 0x27;
	input3[15] = 0x89;
	setValid(cache, 6, 1);
	setTag(cache, getTag(cache, 0x61cfff69), 6);
	setData(cache, input3, 6, 16, 0);


	input4[0] = 0x73;
	input4[1] = 0x74;
	input4[2] = 0x3b;
	input4[3] = 0x09;
	input4[4] = 0xb9;
	input4[5] = 0xe6;
	input4[6] = 0xda;
	input4[7] = 0xf1;
	input4[8] = 0xa4;
	input4[9] = 0xc2;
	input4[10] = 0xe2;
	input4[11] = 0x98;
	input4[12] = 0x2b;
	input4[13] = 0xf3;
	input4[14] = 0x2f;
	input4[15] = 0xd9;
	setValid(cache, 7, 1);
	setTag(cache, getTag(cache, 0x61c77377), 7);
	setData(cache, input4, 7, 16, 0);

	CU_ASSERT_EQUAL(writeByte(cache, 0x61c77377, 0x61), 0);
	input4[7] = 0x61;
	CU_ASSERT_EQUAL(writeByte(cache, 0x61c11130, 0xff), 0);
	input2[0] = 0xff;
	CU_ASSERT_EQUAL(writeByte(cache, 0x61c0000b, 0xa9), 0);
	input1[11] = 0xa9;
	CU_ASSERT_EQUAL(writeByte(cache, 0x61cfff6f, 0x70), 0);
	input3[15] = 0x70;

	block = fetchBlock(cache, 0);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input1[i]);
	}
	free(block);

	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	block = fetchBlock(cache, 6);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input3[i]);
	}
	free(block);

	block = fetchBlock(cache, 7);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input4[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(getDirty(cache, 0), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 3), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 6), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 7), 1);

	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61c77370, 0x9243), 0);
	input4[0] = 0x92;
	input4[1] = 0x43;
	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61c1113e, 0x2644), 0);
	input2[14] = 0x26;
	input2[15] = 0x44;
	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61c00006, 0x5898), 0);
	input1[6] = 0x58;
	input1[7] = 0x98;
	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61cfff6a, 0x2cbb), 0);
	input3[10] = 0x2c;
	input3[11] = 0xbb;

	block = fetchBlock(cache, 0);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input1[i]);
	}
	free(block);

	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	block = fetchBlock(cache, 6);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input3[i]);
	}
	free(block);

	block = fetchBlock(cache, 7);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input4[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(getDirty(cache, 0), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 3), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 6), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 7), 1);

	CU_ASSERT_EQUAL(writeWord(cache, 0x61c77374, 0x38749327), 0);
	input4[4] = 0x38;
	input4[5] = 0x74;
	input4[6] = 0x93;
	input4[7] = 0x27;
	CU_ASSERT_EQUAL(writeWord(cache, 0x61c11138, 0xa67c263b), 0);
	input2[8] = 0xa6;
	input2[9] = 0x7c;
	input2[10] = 0x26;
	input2[11] = 0x3b;
	CU_ASSERT_EQUAL(writeWord(cache, 0x61c0000c, 0x989a8bbc), 0);
	input1[12] = 0x98;
	input1[13] = 0x9a;
	input1[14] = 0x8b;
	input1[15] = 0xbc;
	CU_ASSERT_EQUAL(writeWord(cache, 0x61cfff60, 0xffffffff), 0);
	input3[0] = 0xff;
	input3[1] = 0xff;
	input3[2] = 0xff;
	input3[3] = 0xff;

	block = fetchBlock(cache, 0);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input1[i]);
	}
	free(block);

	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	block = fetchBlock(cache, 6);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input3[i]);
	}
	free(block);

	block = fetchBlock(cache, 7);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input4[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(getDirty(cache, 0), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 3), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 6), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 7), 1);

	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61c77378, 0x0123456789abcdef), 0);
	input4[8] = 0x01;
	input4[9] = 0x23;
	input4[10] = 0x45;
	input4[11] = 0x67;
	input4[12] = 0x89;
	input4[13] = 0xab;
	input4[14] = 0xcd;
	input4[15] = 0xef;
	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61c11130, 0xfedcba9876543210), 0);
	input2[0] = 0xfe;
	input2[1] = 0xdc;
	input2[2] = 0xba;
	input2[3] = 0x98;
	input2[4] = 0x76;
	input2[5] = 0x54;
	input2[6] = 0x32;
	input2[7] = 0x10;
	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61c00000, 0), 0);
	input1[0] = 0;
	input1[1] = 0;
	input1[2] = 0;
	input1[3] = 0;
	input1[4] = 0;
	input1[5] = 0;
	input1[6] = 0;
	input1[7] = 0;
	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61cfff68, 0xffffffffffffffff), 0);
	input3[8] = 0xff;
	input3[9] = 0xff;
	input3[10] = 0xff;
	input3[11] = 0xff;
	input3[12] = 0xff;
	input3[13] = 0xff;
	input3[14] = 0xff;
	input3[15] = 0xff;

	block = fetchBlock(cache, 0);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input1[i]);
	}
	free(block);

	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	block = fetchBlock(cache, 6);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input3[i]);
	}
	free(block);

	block = fetchBlock(cache, 7);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input4[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(getDirty(cache, 0), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 3), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 6), 1);
	CU_ASSERT_EQUAL(getDirty(cache, 7), 1);



	//Verify that writes that don't succeed don't write to the cache
	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61c77377, 0x4545), -1);
	block = fetchBlock(cache, 7);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input4[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(writeWord(cache, 0x61cfff6a, 0x4545), -1);
	block = fetchBlock(cache, 6);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input3[i]);
	}
	free(block);
	
	//printCache(cache);
	
	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61c0000c, 0x4545), -1);
	block = fetchBlock(cache, 0);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input1[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(writeByte(cache, 0x61b11131, 0x45), -1);
	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61b11131, 0x45), -1);
	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(writeWord(cache, 0x61b11131, 0x45), -1);
	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);

	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61b11131, 0x45), -1);
	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], input2[i]);
	}
	free(block);



	//Make writes that are not in cache and verify eviction writes back to main memory
	CU_ASSERT_EQUAL(writeByte(cache, 0x61cbbb59, 0x55), 0);
	mem = readFromMem(cache, 0x61cbbb50);
	mem[9] = 0x55;
	block = fetchBlock(cache, 5);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(block);
	free(mem);

	CU_ASSERT_EQUAL(writeByte(cache, 0x61cbbb0b, 0xb4), 0);
	mem = readFromMem(cache, 0x61cbbb00);
	mem[11] = 0xb4;
	block = fetchBlock(cache, 0);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(block);
	free(mem);

	mem = readFromMem(cache, 0x61c00000);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input1[i], mem[i]);
	}
	free(mem);

	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61caaa3e, 0xacdc), 0);
	mem = readFromMem(cache, 0x61caaa30);
	mem[14] = 0xac;
	mem[15] = 0xdc;
	block = fetchBlock(cache, 3);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(block);
	free(mem);

	mem = readFromMem(cache, 0x61c11130);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input2[i], mem[i]);
	}
	free(mem);

	CU_ASSERT_EQUAL(writeWord(cache, 0x61ceee64, 0xabcdef00), 0);
	mem = readFromMem(cache, 0x61ceee60);
	mem[4] = 0xab;
	mem[5] = 0xcd;
	mem[6] = 0xef;
	mem[7] = 0;

	block = fetchBlock(cache, 6);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(block);
	free(mem);

	mem = readFromMem(cache, 0x61cfff60);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input3[i], mem[i]);
	}
	free(mem);

	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61c61c70, 0x0123456789fedcba), 0);
	mem = readFromMem(cache, 0x61c61c70);
	mem[0] = 0x01;
	mem[1] = 0x23;
	mem[2] = 0x45;
	mem[3] = 0x67;
	mem[4] = 0x89;
	mem[5] = 0xfe;
	mem[6] = 0xdc;
	mem[7] = 0xba;
	block = fetchBlock(cache, 7);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(block);
	free(mem);

	mem = readFromMem(cache, 0x61c77370);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(input4[i], mem[i]);
	}
	free(mem);

	deleteCache(cache);
}

void test_writeErrors() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/physicalMemory1.txt";



	
	n = 1;
	blockDataSize = 16;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	CU_ASSERT_EQUAL(writeByte(cache, 0x61bfffff, 0), -1);
	CU_ASSERT_EQUAL(writeByte(cache, 0x61d00000, 0), -1);


	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61bffffe, 0), -1);
	CU_ASSERT_EQUAL(writeHalfWord(cache, 0x61d00000, 0), -1);

	CU_ASSERT_EQUAL(writeWord(cache, 0x61bffffc, 0), -1);
	CU_ASSERT_EQUAL(writeWord(cache, 0x61d00000, 0), -1);

	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61bffff8, 0), -1);
	CU_ASSERT_EQUAL(writeDoubleWord(cache, 0x61d00000, 0), -1);

}

void test_CreateDMCache() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	uint8_t* block;
	wordInfo_t wordVal;
	n = 1;
	memFile = "testFiles/physicalMemory2.txt";

	//Stolen from SP16_mt2
	blockDataSize = 8;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	uint32_t address = 0x61c00000;
	int i, j;
	int total = 0;
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			CU_ASSERT_EQUAL(writeWord(cache, address +  (((i << 3) + j) << 2), i + j), 0);
		}
	}

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			wordVal = readWord(cache, address + (((i << 3) + j) << 2));
			CU_ASSERT_EQUAL(wordVal.success, true);
			total += (int) wordVal.data;
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			CU_ASSERT_EQUAL(getValid(cache, j + (i << 2)), 1);
			CU_ASSERT_EQUAL(getDirty(cache, j + (i << 2)), 0);
			block = fetchBlock(cache, j + (i << 2));
			CU_ASSERT_EQUAL(block[0], 0);
			CU_ASSERT_EQUAL(block[1], 0);
			CU_ASSERT_EQUAL(block[2], 0);
			CU_ASSERT_EQUAL(block[3], 4 + i + (j << 1));
			CU_ASSERT_EQUAL(block[4], 0);
			CU_ASSERT_EQUAL(block[5], 0);
			CU_ASSERT_EQUAL(block[6], 0);
			CU_ASSERT_EQUAL(block[7], 5 + i + (j << 1));
			free(block);
		}
	}
	deleteCache(cache);
}

void test_CreateFACache(){
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	uint8_t* block;
	uint8_t* mem;
	halfWordInfo_t halfVal;
	memFile ="testFiles/physicalMemory3.txt";

	n = 4;
	blockDataSize = 16;
	totalDataSize = 64;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	uint32_t addr1 = 0x61c90000;
	uint32_t addr2 = 0x61c45380;
	int sum = 0;
	for (int i = 0; i < 32; i++) {
		for(int j = 0; j < 32; j++) {
			//printf("i=%d, j=%d",i,j);
			//printCache(cache);
			halfVal = readHalfWord(cache, addr1 +  (((i << 5) + j) << 1));
			CU_ASSERT_EQUAL(halfVal.success, true);
			sum += halfVal.data;
		//	printCache(cache);
			halfVal = readHalfWord(cache, addr2 + (((i << 5) + j) << 1));
			CU_ASSERT_EQUAL(halfVal.success, true);
			sum += halfVal.data;
		}
	}
	//printCache(cache);
	for (int i = 0; i < 4; i++) {
		block = fetchBlock(cache, i);
		if (i & 1) {
			mem = readFromMem(cache, addr2 + 2016 + ((i & 2) << 3)); 
		} else {
			mem = readFromMem(cache, addr1 + 2016 + ((i & 2) << 3));
		}
		for (int j = 0; j < 16; j ++) {
	//		printf("mem[%d]: %x,  block[%d]: %x \n",j,mem[j],j,block[j]);
			CU_ASSERT_EQUAL(mem[j], block[j]);
		}
		free(block);
		free(mem);
 	}
 	deleteCache(cache);
 }


void test_Create4WaysCache() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	uint8_t* block;
	uint8_t* mem;
	wordInfo_t wordVal;
	memFile = "testFiles/physicalMemory4.txt";

	//Adapted from Sp 15 midterm 2
	n = 4;
	blockDataSize = 32;
	totalDataSize = 256;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	uint32_t address = 0x61c00000;
	printCache(cache);
	for (int i = 0; i < 1024; i += 8) {
		CU_ASSERT_EQUAL(writeWord(cache, address + (i << 2), i), 0);
		printCache(cache);
	}
	printCache(cache);
	for (int i = 1024; i >= 0; i -= 8) {
		wordVal = readWord(cache, address + 4 * i);
		CU_ASSERT_EQUAL(wordVal.success, true);
		CU_ASSERT_EQUAL(writeWord(cache, address + ((i + 1) << 2), wordVal.data), 0);
	}
	printCache(cache);

	for (int i = 0; i < 4; i++) {
		CU_ASSERT_EQUAL(getValid(cache, i), 1);
		CU_ASSERT_EQUAL(getDirty(cache, i), 1);
		CU_ASSERT_EQUAL(getLRU(cache, i), i);
		block = fetchBlock(cache, i);
		mem = readFromMem(cache, address + (i << 6));
		CU_ASSERT_EQUAL(block[0], 0);
		CU_ASSERT_EQUAL(block[1], 0);
		CU_ASSERT_EQUAL(block[2], 0);
		CU_ASSERT_EQUAL(block[3], i << 4);
		CU_ASSERT_EQUAL(block[4], 0);
		CU_ASSERT_EQUAL(block[5], 0);
		CU_ASSERT_EQUAL(block[6], 0);
		CU_ASSERT_EQUAL(block[7], i << 4);
		for (int j = 8; j < 32; j++) {
			CU_ASSERT_EQUAL(block[j], mem[j]);
		}
		free(block);
		free(mem);
	}

	for (int i = 0; i < 4; i++) {
		CU_ASSERT_EQUAL(getValid(cache, 4 + i), 1);
		CU_ASSERT_EQUAL(getDirty(cache, 4 + i), 1);
		CU_ASSERT_EQUAL(getLRU(cache, 4 + i), i);
		block = fetchBlock(cache, 4 + i);
		mem = readFromMem(cache, address + 32 + (i << 6));
		CU_ASSERT_EQUAL(block[0], 0);
		CU_ASSERT_EQUAL(block[1], 0);
		CU_ASSERT_EQUAL(block[2], 0);
		CU_ASSERT_EQUAL(block[3], 8 + (i << 4));
		CU_ASSERT_EQUAL(block[4], 0);
		CU_ASSERT_EQUAL(block[5], 0);
		CU_ASSERT_EQUAL(block[6], 0);
		CU_ASSERT_EQUAL(block[7], 8 + (i << 4));
		for (int j = 8; j < 32; j++) {
			CU_ASSERT_EQUAL(block[j], mem[j]);
		}
		free(block);
		free(mem);
	}

	deleteCache(cache);
}


int main(int argc, char** argv) {
	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    pSuite1 = CU_add_suite("Testing individual Functions", NULL, NULL);
    if (!pSuite1) {
        goto exit;
    }
    pSuite2 = CU_add_suite("Testing Whole Cache Operations", NULL, NULL);
    if (!pSuite2) {
        goto exit;
    }
    switch(argc - 1) {
    	case 0:
    		if (!CU_add_test(pSuite1, "test_Mem", test_Mem)) {
        		goto exit;
    		}
    	case 1:
    		if (!CU_add_test(pSuite1, "test_Utils", test_Utils)) {
        		goto exit;
        	}
        	if (!CU_add_test(pSuite1, "test_Locations", test_Locations)) {
        		goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    	case 2:
    		if (!CU_add_test(pSuite1, "test_Bit", test_Bit)) {
       	 		goto exit;
    		}
    		if (!CU_add_test(pSuite1, "test_Getters", test_Getters)) {
        		goto exit;
    		}
    		if (!CU_add_test(pSuite1, "test_Getters_and_Setters", test_Getters_and_Setters)) {
        		goto exit;
    		}
    		if (!CU_add_test(pSuite1, "test_Extraction", test_Extraction)) {
        		goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    	case 3:
    		if (!CU_add_test(pSuite1, "test_Read", test_Read)) {
        		goto exit;
    		}	
    		if (!CU_add_test(pSuite1, "test_ReadErrors", test_ReadErrors)) {
    			goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    	case 4:
    		if (!CU_add_test(pSuite1, "test_Write", test_Write)) {
        		goto exit;
    		}
    		if (!CU_add_test(pSuite1, "test_writeErrors", test_writeErrors)) {
        		goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    	default:
    		if (!CU_add_test(pSuite2, "test_CreateDMCache", test_CreateDMCache)) {
        		goto exit;
 			}
    		if (!CU_add_test(pSuite2, "test_CreateFACache", test_CreateFACache)) {
        		goto exit;
    		}
    		if (!CU_add_test(pSuite2, "test_Create4WaysCache", test_Create4WaysCache)) {
        		goto exit;
    		}
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
exit:
    CU_cleanup_registry();
    return CU_get_error();
}
