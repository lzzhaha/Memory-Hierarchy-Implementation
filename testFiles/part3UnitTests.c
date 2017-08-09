#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "../part1/utils.h"
#include "../part1/setInCache.h"
#include "../part1/getFromCache.h"
#include "../part1/mem.h"
#include "../part1/cacheRead.h"
#include "../part3/coherenceUtils.h"
#include "../part3/coherenceRead.h"
#include "../part3/coherenceWrite.h"

void test_States() {
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

	setTag(cache, getTag(cache, 0x61c00000), 0);

	setDirty(cache, 0, 0);
	setShared(cache, 0, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), INVALID);

	setShared(cache, 0, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), INVALID);

	setDirty(cache, 0, 1);
	setShared(cache, 0, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), INVALID);

	setShared(cache, 0, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), INVALID);

	setValid(cache, 0, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), OWNED);

	setShared(cache, 0, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), MODIFIED);

	setDirty(cache, 0, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), EXCLUSIVE);

	setShared(cache, 0, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), SHARED);

	setTag(cache, getTag(cache, 0x61c00020), 2);

	setDirty(cache, 2, 0);
	setShared(cache, 2, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	setShared(cache, 2, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	setDirty(cache, 2, 1);
	setShared(cache, 2, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	setShared(cache, 2, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	setValid(cache, 2, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), OWNED);

	setShared(cache, 2, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), MODIFIED);

	setDirty(cache, 2, 0);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), EXCLUSIVE);

	setShared(cache, 2, 1);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), SHARED);

	//Test that we can set a state of a block
	setState(cache, 0, INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), INVALID);

	setState(cache, 0, SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), SHARED);

	setState(cache, 0, EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), EXCLUSIVE);

	setState(cache, 0, OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), OWNED);

	setState(cache, 0, MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), MODIFIED);

	setState(cache, 2, INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	setState(cache, 2, SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), SHARED);

	setState(cache, 2, EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), EXCLUSIVE);

	setState(cache, 2, OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), OWNED);

	setState(cache, 2, MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), MODIFIED);

	//Test that States are properly updated
	updateState(cache, 0x61c00003, INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), MODIFIED);

	updateState(cache, 0x61c00005, EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), MODIFIED);

	updateState(cache, 0x61c00003, OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), MODIFIED);

	updateState(cache, 0x61c0000C, SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), OWNED);

	updateState(cache, 0x61c00000, MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00000), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00003), INVALID);

	updateState(cache, 0x61c00020, OWNED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), MODIFIED);

	updateState(cache, 0x61c00029, MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	updateState(cache, 0x61c00020, INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), INVALID);

	setState(cache, 2, EXCLUSIVE);

	updateState(cache, 0x61c00020, SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), SHARED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), SHARED);

	updateState(cache, 0x61c00020, INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), EXCLUSIVE);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), EXCLUSIVE);

	setState(cache, 2, OWNED);

	updateState(cache, 0x61c00020, INVALID);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00020), MODIFIED);
	CU_ASSERT_EQUAL(determineState(cache, 0x61c00023), MODIFIED);


	deleteCache(cache);
}

void test_CoherenceReads() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cacheSystem_t* sys;
	cacheNode_t** lst;
	uint8_t* mem;
	byteInfo_t byteVal;
	halfWordInfo_t halfVal;
	wordInfo_t wordVal;
	doubleWordInfo_t doubleVal;

	n = 1;
	blockDataSize = 16;
	totalDataSize = 256;
	memFile = "testFiles/physicalMemory1.txt";
	lst = malloc(sizeof(cacheNode_t*) * 2);
	lst[0] = createCacheNode(createCache(n, blockDataSize, totalDataSize, memFile), 1);
	lst[1] = createCacheNode(createCache(n, blockDataSize, totalDataSize, memFile), 2);
	sys = createCacheSystem(lst, 2, createSnooper());
	CU_ASSERT_PTR_NOT_NULL(sys);

	//Test the read by explicitly testing the FSMs that do not involve dirty for each function

	//Work with address 0x61c00050
	cache_t* cache = getCacheFromID(sys, 1);
	mem = readFromMem(cache, 0x61c00050);
	byteVal = cacheSystemByteRead(sys, 0x61c00050, 1);
	CU_ASSERT_EQUAL(byteVal.data, mem[0]);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c00050)); //Verify Read from invalid when no other cache has info
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00050)); //Verify ProbeRead from invalid



	byteVal = cacheSystemByteRead(sys, 0x61c00050, 2);
	CU_ASSERT_EQUAL(byteVal.data, mem[0]);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c00050)); //Verify ProbeRead from Exclusive
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c00050)); //Verify Read from Invalid when other cache has info

	byteVal = cacheSystemByteRead(sys, 0x61c00052, 2);
	CU_ASSERT_EQUAL(byteVal.data, mem[2]);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c00050));
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c00050)); //Verify read from shared

	//Test other reads that evict cache contents and shared -> exclusive
	byteVal = cacheSystemByteRead(sys, 0x61c10050, 1);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c00050)); //Verify eviction updates
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 2), 0x61c00050));

	byteVal = cacheSystemByteRead(sys, 0x61c00057, 2);
	CU_ASSERT_EQUAL(byteVal.data, mem[7]);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c00050));
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 2), 0x61c00050)); //Verify read from exclusive

	free(mem);


	mem = readFromMem(cache, 0x61c20050);
	uint16_t halfData = mem[0];
	halfData = (halfData << 8) | mem[1];
	halfVal = cacheSystemHalfWordRead(sys, 0x61c20050, 2);
	CU_ASSERT_EQUAL(halfVal.data, halfData);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c20050)); 
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 2), 0x61c20050)); 

	halfVal = cacheSystemHalfWordRead(sys, 0x61c20050, 2);
	CU_ASSERT_EQUAL(halfVal.data, halfData);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c20050));
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 2), 0x61c20050));

	halfVal = cacheSystemHalfWordRead(sys, 0x61c20050, 1);
	CU_ASSERT_EQUAL(halfVal.data, halfData);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c20050)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c20050)); 

	halfVal = cacheSystemHalfWordRead(sys, 0x61c20050, 2);
	CU_ASSERT_EQUAL(halfVal.data, halfData);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c20050));
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c20050)); 

	//Test other reads that evict cache contents and shared -> exclusive
	halfVal = cacheSystemHalfWordRead(sys, 0x61c40050, 2);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c20050)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c20050));

	free(mem);



	mem = readFromMem(cache, 0x61ca0050);
	uint32_t wordData = 0;
	for (int i = 0; i < 4; i++) {
		wordData = (wordData << 8) | mem[i];
	}
	wordVal = cacheSystemWordRead(sys, 0x61ca0050, 1);
	CU_ASSERT_EQUAL(wordVal.data, wordData);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61ca0050)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61ca0050)); 


	wordVal = cacheSystemWordRead(sys, 0x61ca0050, 1);
	CU_ASSERT_EQUAL(wordVal.data, wordData);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61ca0050));
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61ca0050));

	wordVal = cacheSystemWordRead(sys, 0x61ca0050, 2);
	CU_ASSERT_EQUAL(wordVal.data, wordData);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61ca0050)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61ca0050)); 

	wordVal = cacheSystemWordRead(sys, 0x61ca0050, 2);
	CU_ASSERT_EQUAL(wordVal.data, wordData);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61ca0050));
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61ca0050));

	//Test other reads that evict cache contents and shared -> exclusive
	wordVal = cacheSystemWordRead(sys, 0x61c80050, 2);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61ca0050)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61ca0050));

	free(mem);


	mem = readFromMem(cache, 0x61cf0050);
	uint64_t doubleData = 0;
	for (int i = 0; i < 8; i++) {
		doubleData = (doubleData << 8) | mem[i];
	}
	doubleVal = cacheSystemDoubleWordRead(sys, 0x61cf0050, 2);
	CU_ASSERT_EQUAL(doubleVal.data, doubleData);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61cf0050)); 
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 2), 0x61cf0050)); 

	doubleVal = cacheSystemDoubleWordRead(sys, 0x61cf0050, 1);
	CU_ASSERT_EQUAL(doubleVal.data, doubleData);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61cf0050)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61cf0050)); 

	doubleVal = cacheSystemDoubleWordRead(sys, 0x61cf0050, 2);
	CU_ASSERT_EQUAL(doubleVal.data, doubleData);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61cf0050));
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61cf0050));

	//Test other reads that evict cache contents and shared -> exclusive
	doubleVal = cacheSystemDoubleWordRead(sys, 0x61cd0050, 2);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61cf0050)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cf0050));


	doubleVal = cacheSystemDoubleWordRead(sys, 0x61cf0050, 1);
	CU_ASSERT_EQUAL(doubleVal.data, doubleData);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61cf0050));
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cf0050));

	free(mem);
	deleteCacheSystem(sys);
}

void test_CoherenceWrites() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cacheSystem_t* sys;
	cacheNode_t** lst;
	uint8_t input1[16];
	uint16_t halfData;
	uint16_t halfMem;
	uint32_t wordData;
	uint32_t wordMem;
	uint64_t doubleData;
	uint64_t doubleMem;
	uint8_t* block;
	uint8_t* mem;

	n = 1;
	blockDataSize = 16;
	totalDataSize = 256;
	memFile = "testFiles/physicalMemory1.txt";
	lst = malloc(sizeof(cacheNode_t*) * 2);
	lst[0] = createCacheNode(createCache(n, blockDataSize, totalDataSize, memFile), 1);
	lst[1] = createCacheNode(createCache(n, blockDataSize, totalDataSize, memFile), 2);
	sys = createCacheSystem(lst, 2, createSnooper());
	CU_ASSERT_PTR_NOT_NULL(sys);

	input1[0] = 0x1c;
	input1[1] = 0xd4;
	input1[2] = 0x87;
	input1[3] = 0xe3;
	input1[4] = 0xcc;
	input1[5] = 0x31;
	input1[6] = 0x2e;
	input1[7] = 0x90;
	input1[8] = 0xcd;
	input1[9] = 0x5f;
	input1[10] = 0x23;
	input1[11] = 0xa1;
	input1[12] = 0x11;
	input1[13] = 0x09;
	input1[14] = 0xe4;
	input1[15] = 0x7d;


	CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, 0x61c00000, 1, input1[0]), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	CU_ASSERT_EQUAL(block[0], input1[0]);
	free(block);

	cacheSystemByteRead(sys, 0x61c00000, 1);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));

	CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, 0x61c00000, 1, input1[0]), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));

	cacheSystemByteRead(sys, 0x61c00000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c00000));
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	CU_ASSERT_EQUAL(block[0], input1[0]);
	free(block);

	cacheSystemByteRead(sys, 0x61c00000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c00000));

	cacheSystemByteRead(sys, 0x61c00000, 1);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c00000));


	CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, 0x61c00000, 1, input1[2]), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	CU_ASSERT_EQUAL(block[0], input1[2]);
	free(block);

	cacheSystemByteRead(sys, 0x61c00000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c00000));

	CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, 0x61c00000, 2, input1[4]), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c00000));
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	CU_ASSERT_EQUAL(block[0], input1[4]);
	free(block);

	cacheSystemByteRead(sys, 0x61c00000, 1);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 2), 0x61c00000));

	cacheSystemByteRead(sys, 0x61c10000, 1);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c00000));

	//SHOW MEMORY IS STILL NOT UPDATED

	mem = readFromMem(getCacheFromID(sys, 2), 0x61c00000);
	block = fetchBlock(getCacheFromID(sys, 2), 0);

	CU_ASSERT_NOT_EQUAL(mem[0], block[0]);
	for (int i = 1; i < 16; i++) {
		CU_ASSERT_EQUAL(mem[i], block[i]);
	}
	free(mem);



	cacheSystemByteRead(sys, 0x61c10000, 2);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));
	//SHOW MEMORY IS UPDATED

	mem = readFromMem(getCacheFromID(sys, 2), 0x61c00000);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(mem[i], block[i]);
	}
	free(mem);
	free(block);

	cacheSystemByteRead(sys, 0x61c00000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));

	CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, 0x61c00000, 1, input1[0]), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));

	cacheSystemByteRead(sys, 0x61c10000, 1);
	cacheSystemByteRead(sys, 0x61c00000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c00000));

	CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, 0x61c00000, 2, input1[0]), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c00000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c00000));

	/* HALFWORD */
	halfData = (input1[0] << 8) | input1[1];

	CU_ASSERT_EQUAL(cacheSystemHalfWordWrite(sys, 0x61c40000, 1, halfData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	halfMem = (block[0] << 8) | block[1];
	CU_ASSERT_EQUAL(halfData, halfMem);
	free(block);


	cacheSystemHalfWordRead(sys, 0x61c40000, 1);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));

	CU_ASSERT_EQUAL(cacheSystemHalfWordWrite(sys, 0x61c40000, 1, halfData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));

	cacheSystemHalfWordRead(sys, 0x61c40000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c40000));
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	halfMem = (block[0] << 8) | block[1];
	CU_ASSERT_EQUAL(halfData, halfMem);
	free(block);

	cacheSystemHalfWordRead(sys, 0x61c40000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	cacheSystemHalfWordRead(sys, 0x61c40000, 1);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	halfData = (input1[9] << 8) | input1[10];

	CU_ASSERT_EQUAL(cacheSystemHalfWordWrite(sys, 0x61c40000, 1, halfData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	halfMem = (block[0] << 8) | block[1];
	CU_ASSERT_EQUAL(halfData, halfMem);
	free(block);


	cacheSystemHalfWordRead(sys, 0x61c40000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	CU_ASSERT_EQUAL(cacheSystemHalfWordWrite(sys, 0x61c40000, 2, halfData), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	cacheSystemHalfWordRead(sys, 0x61c40000, 1);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	cacheSystemHalfWordRead(sys, 0x61c10000, 1);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	//SHOW MEMORY IS STILL NOT UPDATED
	mem = readFromMem(getCacheFromID(sys, 2), 0x61c40000);
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	for (int i = 0; i < 2; i++) {
		CU_ASSERT_NOT_EQUAL(block[i], mem[i]);
	}
	for (int i = 2; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(mem);

	cacheSystemHalfWordRead(sys, 0x61c10000, 2);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));
	//SHOW MEMORY IS UPDATED
	mem = readFromMem(getCacheFromID(sys, 2), 0x61c40000);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(block[i], mem[i]);
	}
	free(mem);
	free(block);

	cacheSystemHalfWordRead(sys, 0x61c40000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));

	CU_ASSERT_EQUAL(cacheSystemHalfWordWrite(sys, 0x61c40000, 1, halfData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));

	cacheSystemHalfWordRead(sys, 0x61c10000, 1);
	cacheSystemHalfWordRead(sys, 0x61c40000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c40000));

	CU_ASSERT_EQUAL(cacheSystemHalfWordWrite(sys, 0x61c40000, 2, halfData), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c40000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c40000));

	/* WORD */
	wordData = (input1[0] << 24) | (input1[1] << 16) | (input1[2] << 8) | input1[3];

	CU_ASSERT_EQUAL(cacheSystemWordWrite(sys, 0x61c80000, 1, wordData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	wordMem = (block[0] << 24) | (block[1] << 16) | (block[2] << 8) | block[3];
	CU_ASSERT_EQUAL(wordData, wordMem);
	free(block);

	cacheSystemWordRead(sys, 0x61c80000, 1);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));

	CU_ASSERT_EQUAL(cacheSystemWordWrite(sys, 0x61c80000, 1, wordData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));

	cacheSystemWordRead(sys, 0x61c80000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c80000));
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	wordMem = (block[0] << 24) | (block[1] << 16) | (block[2] << 8) | block[3];
	CU_ASSERT_EQUAL(wordData, wordMem);
	free(block);

	cacheSystemWordRead(sys, 0x61c80000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c80000));

	cacheSystemWordRead(sys, 0x61c80000, 1);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c80000));

	wordData = (input1[12] << 24) | (input1[13] << 16) | (input1[14] << 8) | input1[15];

	CU_ASSERT_EQUAL(cacheSystemWordWrite(sys, 0x61c80000, 1, wordData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	wordMem = (block[0] << 24) | (block[1] << 16) | (block[2] << 8) | block[3];
	CU_ASSERT_EQUAL(wordData, wordMem);
	free(block);

	cacheSystemWordRead(sys, 0x61c80000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61c80000));

	CU_ASSERT_EQUAL(cacheSystemWordWrite(sys, 0x61c80000, 2, wordData), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c80000));
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	wordMem = (block[0] << 24) | (block[1] << 16) | (block[2] << 8) | block[3];
	CU_ASSERT_EQUAL(wordData, wordMem);
	free(block);

	cacheSystemWordRead(sys, 0x61c80000, 1);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 2), 0x61c80000));

	cacheSystemWordRead(sys, 0x61c10000, 1);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c80000));

	//SHOW MEMORY IS STILL NOT UPDATED
	mem = readFromMem(getCacheFromID(sys, 2), 0x61c80000);
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	for (int i = 0; i < 4; i++) {
		CU_ASSERT_NOT_EQUAL(mem[i], block[i]);
	}
	for (int i = 4; i < 16; i++) {
		CU_ASSERT_EQUAL(mem[i], block[i]);
	}
	free(mem);


	cacheSystemWordRead(sys, 0x61c10000, 2);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));
	//SHOW MEMORY IS UPDATED
	mem = readFromMem(getCacheFromID(sys, 2), 0x61c80000);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(mem[i], block[i]);
	}
	free(mem);
	free(block);

	cacheSystemWordRead(sys, 0x61c80000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));

	CU_ASSERT_EQUAL(cacheSystemWordWrite(sys, 0x61c80000, 1, wordData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));

	cacheSystemWordRead(sys, 0x61c10000, 1);
	cacheSystemWordRead(sys, 0x61c80000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61c80000));

	CU_ASSERT_EQUAL(cacheSystemWordWrite(sys, 0x61c80000, 2, wordData), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61c80000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61c80000));

	/*Double Word*/

	doubleData = 0;
	for (int i = 0; i < 8; i++) {
		doubleData = (doubleData << 8) | input1[i];
	}

	CU_ASSERT_EQUAL(cacheSystemDoubleWordWrite(sys, 0x61cc0000, 1, doubleData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));
	block = fetchBlock(getCacheFromID(sys, 1), 0);
	doubleMem = 0;
	for (int i = 0; i < 8; i++) {
		doubleMem = (doubleMem << 8) | block[i];
	}
	CU_ASSERT_EQUAL(doubleData, doubleMem);
	free(block);


	cacheSystemDoubleWordRead(sys, 0x61cc0000, 1);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	CU_ASSERT_EQUAL(cacheSystemDoubleWordWrite(sys, 0x61cc0000, 1, doubleData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	cacheSystemDoubleWordRead(sys, 0x61cc0000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61cc0000));
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	doubleMem = 0;
	for (int i = 0; i < 8; i++) {
		doubleMem = (doubleMem << 8) | block[i];
	}
	CU_ASSERT_EQUAL(doubleData, doubleMem);
	free(block);

	cacheSystemDoubleWordRead(sys, 0x61cc0000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	cacheSystemDoubleWordRead(sys, 0x61cc0000, 1);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	doubleData = 0;
	for (int i = 0; i < 8; i++) {
		doubleData = (doubleData << 8) | input1[i + 8];
	}

	CU_ASSERT_EQUAL(cacheSystemDoubleWordWrite(sys, 0x61cc0000, 1, doubleData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	block = fetchBlock(getCacheFromID(sys, 1), 0);
	doubleMem = 0;
	for (int i = 0; i < 8; i++) {
		doubleMem = (doubleMem << 8) | block[i];
	}
	CU_ASSERT_EQUAL(doubleData, doubleMem);
	free(block);

	cacheSystemDoubleWordRead(sys, 0x61cc0000, 2);
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	CU_ASSERT_EQUAL(cacheSystemDoubleWordWrite(sys, 0x61cc0000, 2, doubleData), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	block = fetchBlock(getCacheFromID(sys, 2), 0);
	doubleMem = 0;
	for (int i = 0; i < 8; i++) {
		doubleMem = (doubleMem << 8) | block[i];
	}
	CU_ASSERT_EQUAL(doubleData, doubleMem);
	free(block);

	cacheSystemDoubleWordRead(sys, 0x61cc0000, 1);
	CU_ASSERT_EQUAL(SHARED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(OWNED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	cacheSystemDoubleWordRead(sys, 0x61c10000, 1);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	//SHOW MEMORY IS STILL NOT UPDATED
	mem = readFromMem(getCacheFromID(sys, 2), 0x61cc0000);
	block = fetchBlock(getCacheFromID(sys, 2), 0);
	for (int i = 0; i < 8; i++) {
		CU_ASSERT_NOT_EQUAL(mem[i], block[i]);
	}
	for (int i = 8; i < 16; i++) {
		CU_ASSERT_EQUAL(mem[i], block[i]);
	}
	free(mem);


	cacheSystemDoubleWordRead(sys, 0x61c10000, 2);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));
	//SHOW MEMORY IS UPDATED
	mem = readFromMem(getCacheFromID(sys, 2), 0x61cc0000);
	for (int i = 0; i < 16; i++) {
		CU_ASSERT_EQUAL(mem[i], block[i]);
	}
	free(mem);
	free(block);

	cacheSystemDoubleWordRead(sys, 0x61cc0000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	CU_ASSERT_EQUAL(cacheSystemDoubleWordWrite(sys, 0x61cc0000, 1, doubleData), 0);
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	cacheSystemDoubleWordRead(sys, 0x61c10000, 1);
	cacheSystemDoubleWordRead(sys, 0x61cc0000, 1);
	CU_ASSERT_EQUAL(EXCLUSIVE, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	CU_ASSERT_EQUAL(cacheSystemDoubleWordWrite(sys, 0x61cc0000, 2, doubleData), 0);
	CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), 0x61cc0000)); 
	CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), 0x61cc0000));

	deleteCacheSystem(sys);
	
	
}


void test_TwoCaches() {
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
	lst[0] = createCacheNode(createCache(n, blockDataSize, totalDataSize, memFile), 1);
	lst[1] = createCacheNode(createCache(n, blockDataSize, totalDataSize, memFile), 2);
	sys = createCacheSystem(lst, 2, createSnooper());
	CU_ASSERT_PTR_NOT_NULL(sys);
	//Create a two cache system
	for (unsigned int i = 0; i < 72; i++) {
		if (i & 1) {
			byteVal = cacheSystemByteRead(sys, addr + ((i >> 1) + 36), 2);
			CU_ASSERT_EQUAL(byteVal.success, true);
			CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, addr + ((i >> 1) + 36), 2, (uint8_t)  byteVal.data + 8), 0);
		} else {
			byteVal = cacheSystemByteRead(sys, addr + (i >> 1), 1);
			CU_ASSERT_EQUAL(byteVal.success, true);
			CU_ASSERT_EQUAL(cacheSystemByteWrite(sys, addr + (i >> 1), 1, (uint8_t)  byteVal.data + 8), 0);
		}
	}
	for (j = 0; j < 5; j++) {
		cache = getCacheFromID(sys, 1);
		CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 1), addr + (j << 3)));
		CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 2), addr + (j << 3)));
		block = fetchBlock(cache, j);
		mem = readFromMem(cache, addr + (j << 3));
		for (unsigned int i = 0; i < blockDataSize; i++) {	
			CU_ASSERT_EQUAL(block[i], (mem[i] + 8) & UINT8_MAX);
		}
		free(block);
		free(mem);
	}
	for (;j < 9; j++) {
		cache = getCacheFromID(sys, 2);
		CU_ASSERT_EQUAL(INVALID, determineState(getCacheFromID(sys, 1), addr + (j << 3)));
		CU_ASSERT_EQUAL(MODIFIED, determineState(getCacheFromID(sys, 2), addr + (j << 3)));
		block = fetchBlock(cache, j);
		mem = readFromMem(cache, addr + (j << 3));
		for (unsigned int i = 0; i < blockDataSize; i++) {
			CU_ASSERT_EQUAL(block[i], mem[i] + 8);
		}
		free(block);
		free(mem);
	}
	deleteCacheSystem(sys);
}

int main(int argc, char** argv) {
	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    pSuite1 = CU_add_suite("Testing Individual Functions", NULL, NULL);
    if (!pSuite1) {
        goto exit;
    }
    pSuite2 = CU_add_suite("Testing Mutliple Caches", NULL, NULL);
    if (!pSuite2) {
    	goto exit;
    }
    switch(argc - 1) {
    	case 0:
    		if (!CU_add_test(pSuite2, "test_TwoCaches", test_TwoCaches)) {
        		goto exit;
 			}
    	case 1:
    		if (!CU_add_test(pSuite1, "test_States", test_States)) {
        		goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    	case 2:
    		if (!CU_add_test(pSuite1, "test_CoherenceReads", test_CoherenceReads)) {
        		goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    	case 3:
    		if (!CU_add_test(pSuite1, "test_CoherenceWrites", test_CoherenceWrites)) {
        		goto exit;
    		}
    		if (argc - 1) {
    			break;
    		}
    }
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
exit:
    CU_cleanup_registry();
    return CU_get_error();


}
