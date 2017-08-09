#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdint.h>
#include "../part1/utils.h"
#include "../part1/setInCache.h"
#include "../part1/cacheRead.h"
#include "../part1/cacheWrite.h"
#include "../part1/mem.h"
#include "../part2/hitRate.h"
#include "../part2/problem1.h"
#include "../part2/problem2.h"
#include "../part2/problem3.h"

/*
	Tests basic hit rate from a series of hits/misses without a pattern.	
*/
void test_BasicHitRate() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	char* memFile;
	cache_t* cache;
	memFile = "testFiles/physicalMemory1.txt";

	//Verify Read Hit Rates
	n = 1;
	blockDataSize = 8;
	totalDataSize = 32;

	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	readByte(cache, 0x61c00004);
	readByte(cache, 0x61c00005);
	readByte(cache, 0x61c00068);
	readByte(cache, 0x61c000c8);
	readByte(cache, 0x61c00068);
	readByte(cache, 0x61c000dd);
	readByte(cache, 0x61c00045);
	readByte(cache, 0x61c00004);
	readByte(cache, 0x61c000c8);

	CU_ASSERT_EQUAL(findHitRate(cache), 1.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 1);
	clearCache(cache);

	readHalfWord(cache, 0x61c00004);
	readHalfWord(cache, 0x61c00006);
	readHalfWord(cache, 0x61c00068);
	readHalfWord(cache, 0x61c000c8);
	readHalfWord(cache, 0x61c00068);
	readHalfWord(cache, 0x61c000de);
	readHalfWord(cache, 0x61c00046);
	readHalfWord(cache, 0x61c00004);
	readHalfWord(cache, 0x61c000c8);

	CU_ASSERT_EQUAL(findHitRate(cache), 1.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 1);
	clearCache(cache);

	readWord(cache, 0x61c00004);
	readWord(cache, 0x61c00004);
	readWord(cache, 0x61c00068);
	readWord(cache, 0x61c000c8);
	readWord(cache, 0x61c00068);
	readWord(cache, 0x61c000dc);
	readWord(cache, 0x61c00044);
	readWord(cache, 0x61c00004);
	readWord(cache, 0x61c000c8);

	CU_ASSERT_EQUAL(findHitRate(cache), 1.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 1);
	clearCache(cache);

	readDoubleWord(cache, 0x61c00000);
	readDoubleWord(cache, 0x61c00000);
	readDoubleWord(cache, 0x61c00068);
	readDoubleWord(cache, 0x61c000c8);
	readDoubleWord(cache, 0x61c00068);
	readDoubleWord(cache, 0x61c000d8);
	readDoubleWord(cache, 0x61c00040);
	readDoubleWord(cache, 0x61c00000);
	readDoubleWord(cache, 0x61c000c8);

	CU_ASSERT_EQUAL(findHitRate(cache), 1.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 1);
	deleteCache(cache);	

	//Verify Write Hit Rates
	n = 2;
	blockDataSize = 8;
	totalDataSize = 32;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	writeByte(cache, 0x61c00004, 0);
	writeByte(cache, 0x61c00005, 0);
	writeByte(cache, 0x61c00068, 0);
	writeByte(cache, 0x61c000c8, 0);
	writeByte(cache, 0x61c00068, 0);
	writeByte(cache, 0x61c000dd, 0);
	writeByte(cache, 0x61c00045, 0);
	writeByte(cache, 0x61c00004, 0);
	writeByte(cache, 0x61c000c8, 0);

	CU_ASSERT_EQUAL(findHitRate(cache), 3.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 3);
	clearCache(cache);

	writeHalfWord(cache, 0x61c00004, 0);
	writeHalfWord(cache, 0x61c00006, 0);
	writeHalfWord(cache, 0x61c00068, 0);
	writeHalfWord(cache, 0x61c000c8, 0);
	writeHalfWord(cache, 0x61c00068, 0);
	writeHalfWord(cache, 0x61c000de, 0);
	writeHalfWord(cache, 0x61c00046, 0);
	writeHalfWord(cache, 0x61c00004, 0);
	writeHalfWord(cache, 0x61c000c8, 0);

	CU_ASSERT_EQUAL(findHitRate(cache), 3.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 3);
	clearCache(cache);

	writeWord(cache, 0x61c00004, 0);
	writeWord(cache, 0x61c00004, 0);
	writeWord(cache, 0x61c00068, 0);
	writeWord(cache, 0x61c000c8, 0);
	writeWord(cache, 0x61c00068, 0);
	writeWord(cache, 0x61c000dc, 0);
	writeWord(cache, 0x61c00044, 0);
	writeWord(cache, 0x61c00004, 0);
	writeWord(cache, 0x61c000c8, 0);

	CU_ASSERT_EQUAL(findHitRate(cache), 3.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 3);
	clearCache(cache);

	writeDoubleWord(cache, 0x61c00000, 0);
	writeDoubleWord(cache, 0x61c00000, 0);
	writeDoubleWord(cache, 0x61c00068, 0);
	writeDoubleWord(cache, 0x61c000c8, 0);
	writeDoubleWord(cache, 0x61c00068, 0);
	writeDoubleWord(cache, 0x61c000d8, 0);
	writeDoubleWord(cache, 0x61c00040, 0);
	writeDoubleWord(cache, 0x61c00000, 0);
	writeDoubleWord(cache, 0x61c000c8, 0);

	CU_ASSERT_EQUAL(findHitRate(cache), 3.0/9.0);
	CU_ASSERT_EQUAL(cache->access, 9);
	CU_ASSERT_EQUAL(cache->hit, 3);
	deleteCache(cache);
}
/*
	Will take a simple block size of 128 bytes and a 1024 byte cache DMC
	and iterates through a list doing list[i] += list[i].
	Will also take a block size of 64 bytes and a 512 byte cache 4 way
	set associative and incrementing i by 4 each time will do
	list[i] += list[i].	
*/
void test_MediumHitRate() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	uint32_t address;
	char* memFile;
	cache_t* cache;
	byteInfo_t byteVal;
	uint8_t byteTotal;
	halfWordInfo_t halfVal;
	uint16_t halfTotal;
	wordInfo_t wordVal;
	uint32_t wordTotal;
	doubleWordInfo_t doubleVal;
	uint64_t doubleTotal;
	memFile = "testFiles/physicalMemory1.txt";
	address = 0x61c00080;

	n = 1;
	blockDataSize = 128;
	totalDataSize = 1024;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	for (int i = 0; i < 2048; i++) {
		byteVal = readByte(cache, address + i);
		byteTotal += byteVal.data;
		byteVal = readByte(cache, address + i);
		byteTotal += byteVal.data;
		writeByte(cache, address + i, byteTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 383.0/384.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6128);
	clearCache(cache);
	
//	printCache(cache);
	for (int i = 0; i < 2048; i++) {
//		printf("i=%d\n",i);
//		printCache(cache);
		halfVal = readHalfWord(cache, address + (i << 1));
		halfTotal += halfVal.data;
//		printCache(cache);
///		printCache(cache);
		halfVal = readHalfWord(cache, address + (i << 1));
		halfTotal += halfVal.data;
		writeHalfWord(cache, address + (i << 1), halfTotal);
//		printCache(cache);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 191.0/192.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6112);
	clearCache(cache);

	for (int i = 0; i < 2048; i++) {
		wordVal = readWord(cache, address + (i << 2));
		wordTotal += wordVal.data;
		wordVal = readWord(cache, address + (i << 2));
		wordTotal += wordVal.data;
		writeWord(cache, address + (i << 2), wordTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 95.0/96.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6080);
	clearCache(cache);

	for (int i = 0; i < 2048; i++) {
		doubleVal = readDoubleWord(cache, address + (i << 3));
		doubleTotal += doubleVal.data;
		doubleVal = readDoubleWord(cache, address + (i << 3));
		doubleTotal += doubleVal.data;
		writeDoubleWord(cache, address + (i << 3), doubleTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 47.0/48.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6016);
	deleteCache(cache);


	n = 2;
	blockDataSize = 64;
	totalDataSize = 512;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);
	for (int i = 0; i < 2048; i++) {
		byteVal = readByte(cache, address + i);
		byteTotal += byteVal.data;
		byteVal = readByte(cache, address + i);
		byteTotal += byteVal.data;
		writeByte(cache, address + i, byteTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 191.0/192.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6112);
	clearCache(cache);

	for (int i = 0; i < 2048; i++) {
		halfVal = readHalfWord(cache, address + (i << 1));
		halfTotal += halfVal.data;
		halfVal = readHalfWord(cache, address + (i << 1));
		halfTotal += halfVal.data;
		writeHalfWord(cache, address + (i << 1), halfTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 95.0/96.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6080);
	clearCache(cache);

	for (int i = 0; i < 2048; i++) {
		wordVal = readWord(cache, address + (i << 2));
		wordTotal += wordVal.data;
		wordVal = readWord(cache, address + (i << 2));
		wordTotal += wordVal.data;
		writeWord(cache, address + (i << 2), wordTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 47.0/48.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 6016);
	clearCache(cache);

	for (int i = 0; i < 2048; i++) {
		doubleVal = readDoubleWord(cache, address + (i << 3));
		doubleTotal += doubleVal.data;
		doubleVal = readDoubleWord(cache, address + (i << 3));
		doubleTotal += doubleVal.data;
		writeDoubleWord(cache, address + (i << 3), doubleTotal);
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 23.0/24.0);
	CU_ASSERT_EQUAL(cache->access, 6144);
	CU_ASSERT_EQUAL(cache->hit, 5888);
	deleteCache(cache);
}

void test_HitRate1() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	uint32_t address;
	char* memFile;
	cache_t* cache;
	wordInfo_t wordVal;
	double access;
	double hit;
	memFile = "testFiles/physicalMemory1.txt";

	//Adapted from Sp16 Midterm 2
	n = 1;
	blockDataSize = 8;
	totalDataSize = 128;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	address = 0x61c00080;
	int i, j;
	int total = 0;

	/*Loop 1*/
	for (i = 0; i < 8; i++) {
		for (j = 0; j < 8; j++) {
			writeWord(cache, address + (((i << 3) + j) << 2), i + j);
		}
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 0.5);
	access = cache->access;
	hit = cache->hit;

	/*Loop 2*/
	for (j = 0; j < 8; j++) {
		for (i = 0; i < 8; i++) {
			wordVal = readWord(cache, address + (((i << 3) + j) << 2));
			total += (int) wordVal.data;
		}
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 0.25);

	/*Local Hit Rate for Loop 2*/
	cache->access = cache->access - access;
	cache->hit = cache->hit - hit;
	CU_ASSERT_EQUAL(findHitRate(cache), 0.0);
	deleteCache(cache);
}

void test_HitRate2() {
	uint32_t n;
	uint32_t blockDataSize;
	uint32_t totalDataSize;
	uint32_t addr1;
	uint32_t addr2;
	char* memFile;
	cache_t* cache;
	wordInfo_t wordVal;
	memFile = "testFiles/physicalMemory2.txt";

	//Adapted from Fa15 Midterm 2
	n = 1;
	blockDataSize = 512;
	totalDataSize = 2048;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	addr1 = 0x61c10000;
	addr2 = 0x61c20000;

	int total = 0;
	for (int i = 0; i < 4096; i += 64) {
		wordVal = readWord(cache, addr1 + (i << 2));
		total += wordVal.data;
		wordVal = readWord(cache, addr2 + (i << 2));
		total -= wordVal.data;
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 0.0);

	clearCache(cache);

	addr1 = 0x61c10000;
	addr2 = 0x61c10000;

	total = 0;
	for (int i = 0; i < 4096; i += 64) {
		wordVal = readWord(cache, addr1 + (i << 2));
		total += wordVal.data;
		wordVal = readWord(cache, addr2 + (i << 2));
		total -= wordVal.data;
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 0.75);
	deleteCache(cache);

	n = 4;
	blockDataSize = 512;
	totalDataSize = 2048;
	cache = createCache(n, blockDataSize, totalDataSize, memFile);
	CU_ASSERT_PTR_NOT_NULL(cache);

	addr1 = 0x61c10000;
	addr2 = 0x61c20000;

	total = 0;
	for (int i = 0; i < 4096; i += 64) {
		wordVal = readWord(cache, addr1 + (i << 2));
		total += wordVal.data;
		wordVal = readWord(cache, addr2 + (i << 2));
		total -= wordVal.data;
	}

	CU_ASSERT_EQUAL(findHitRate(cache), 0.50);
	deleteCache(cache);
}



void test_Problem1HitRate() {
	cache_t* cache = params1();
	CU_ASSERT_PTR_NOT_NULL(cache);
	if (cache != NULL) {
		deleteCache(cache);
	}
}

void test_Problem2HitRate() {
	cache_t* cache = params2();
	CU_ASSERT_PTR_NOT_NULL(cache);
	if (cache != NULL) {
		deleteCache(cache);
	}
}

void test_Problem3HitRate() {
	cache_t* cache = params3();
	CU_ASSERT_PTR_NOT_NULL(cache);
	if (cache != NULL) {
		deleteCache(cache);
	}
}


int main() {
	CU_pSuite pSuite1 = NULL;
	CU_pSuite pSuite2 = NULL;
	CU_pSuite pSuite3 = NULL;
	CU_pSuite pSuite4 = NULL;
	if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }
    pSuite1 = CU_add_suite("Testing Hit Rate", NULL, NULL);
    if (!pSuite1) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_BasicHitRate", test_BasicHitRate)) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_MediumHitRate", test_MediumHitRate)) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_HitRate1", test_HitRate1)) {
        goto exit;
    }
    if (!CU_add_test(pSuite1, "test_HitRate2", test_HitRate2)) {
        goto exit;
    }
    pSuite2 = CU_add_suite("Testing Problem 1", NULL, NULL);
    if (!CU_add_test(pSuite2, "test_Problem1HitRate", test_Problem1HitRate)) {
        goto exit;
 	}
    pSuite3 = CU_add_suite("Testing Problem 2", NULL, NULL);
    if (!CU_add_test(pSuite3, "test_Problem2HitRate", test_Problem2HitRate)) {
        goto exit;
 	}

 	pSuite4 = CU_add_suite("Testing Problem 3", NULL, NULL);
    if (!CU_add_test(pSuite4, "test_Problem3HitRate", test_Problem3HitRate)) {
        goto exit;
 	}
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    
exit:
    CU_cleanup_registry();
    return CU_get_error();

}
