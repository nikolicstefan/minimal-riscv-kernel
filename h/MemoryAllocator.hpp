#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:
    struct MemSegment {
        MemSegment *next;
        MemSegment *prev;
        size_t size; // size in bytes
    };

    static void memInit();

    static void *memAlloc(size_t size); // size in blocks

    static int memFree(void *addr);

    static size_t memGetFreeSpace();

    static size_t memGetLargestFreeBlock();

private:
    static MemSegment *freeMem, *allocMem;

    static int memTryToJoin(MemSegment *memSeg);

};

#endif // MEMORYALLOCATOR_HPP
