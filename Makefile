CC = gcc
CFLAGS = -g -std=gnu99 -Wall
CUNIT = -L/home/ff/cs61c/cunit/install/lib -I/home/ff/cs61c/cunit/install/include -lcunit

test-all: test-part1 test-part2 test-part3

memCheck: part1-memCheck part2-memCheck part3-memCheck

clean:
	rm -f *.o caches
	rm -f testFiles/10AddressTest.txt
	rm -f testFiles/50AddressTest.txt
	rm -f testFiles/100AddressTest.txt
	rm -f testFiles/physicalMemory1.txt
	rm -f testFiles/physicalMemory2.txt
	rm -f testFiles/physicalMemory3.txt
	rm -f testFiles/physicalMemory4.txt


copy:
	cp dataSets/10AddressTest.txt testFiles/10AddressTest.txt
	cp dataSets/50AddressTest.txt testFiles/50AddressTest.txt
	cp dataSets/100AddressTest.txt testFiles/100AddressTest.txt
	cp dataSets/physicalMemory1.txt testFiles/physicalMemory1.txt
	cp dataSets/physicalMemory2.txt testFiles/physicalMemory2.txt
	cp dataSets/physicalMemory3.txt testFiles/physicalMemory3.txt
	cp dataSets/physicalMemory4.txt testFiles/physicalMemory4.txt

part1: clean copy
	$(CC) $(CFLAGS) -DTESTING -o caches testFiles/part1UnitTests.c part1/utils.c part1/setInCache.c part1/mem.c part1/getFromCache.c part1/cacheWrite.c part1/cacheRead.c part2/hitRate.c $(CUNIT) -lm

part2: clean copy
	$(CC) $(CFLAGS) -DTESTING -o caches testFiles/part2UnitTests.c part1/utils.c part1/setInCache.c part1/mem.c part1/getFromCache.c part1/cacheWrite.c part1/cacheRead.c part2/hitRate.c part2/problem1.c part2/problem2.c part2/problem3.c $(CUNIT) -lm


part3: clean copy
	$(CC) $(CFLAGS) -DTESTING -o caches testFiles/part3UnitTests.c part1/utils.c part1/setInCache.c part1/mem.c part1/getFromCache.c part1/cacheWrite.c part1/cacheRead.c part2/hitRate.c part2/problem1.c part2/problem2.c part3/coherenceUtils.c part3/coherenceRead.c part3/coherenceWrite.c $(CUNIT) -lm

test-part1: part1
	./caches 

test-part1-utils: part1
	./caches 1

test-part1-get-set: part1
	./caches 2 2

test-part1-read: part1
	./caches 3 3 3

test-part1-write: part1
	./caches 4 4 4 4

test-part2: part2
	./caches

test-part3: part3
	./caches	

test-part3-utils: part3
	./caches 1

test-part3-read: part3
	./caches 2 2

test-part3-write: part3
	./caches 3 3 3

part1-main: clean copy
	$(CC) $(CFLAGS) -DTESTING -o caches part1/part1Main.c part1/utils.c part1/setInCache.c part1/mem.c part1/getFromCache.c part1/cacheWrite.c part1/cacheRead.c part2/hitRate.c $(CUNIT) -lm

part2-main: clean copy
	$(CC) $(CFLAGS) -DTESTING -o caches part2/part2Main.c part1/utils.c part1/setInCache.c part1/mem.c part1/getFromCache.c part1/cacheWrite.c part1/cacheRead.c part2/hitRate.c part2/problem1.c part2/problem2.c part2/problem3.c $(CUNIT) -lm

part3-main: clean copy
	$(CC) $(CFLAGS) -DTESTING -o caches part3/part3Main.c part1/utils.c part1/setInCache.c part1/mem.c part1/getFromCache.c part1/cacheWrite.c part1/cacheRead.c part2/hitRate.c part2/problem1.c part2/problem2.c part3/coherenceUtils.c part3/coherenceRead.c part3/coherenceWrite.c $(CUNIT) -lm

part1-memCheck: part1-main
	valgrind --tool=memcheck --leak-check=full --dsymutil=yes --undef-value-errors=no ./caches

part2-memCheck: part2-main
	valgrind --tool=memcheck --leak-check=full --dsymutil=yes --undef-value-errors=no ./caches

part3-memCheck: part3-main
	valgrind --tool=memcheck --leak-check=full --dsymutil=yes --undef-value-errors=no ./caches