#include "../h/MemoryAllocator.hpp"

MemoryAllocator::MemSegment *MemoryAllocator::freeMem = nullptr;
MemoryAllocator::MemSegment *MemoryAllocator::allocMem = nullptr;

void MemoryAllocator::memInit() {
    freeMem = (MemSegment *)HEAP_START_ADDR;
    freeMem->next = nullptr;
    freeMem->prev = nullptr;
    freeMem->size = (uint8 *)HEAP_END_ADDR - (uint8 *)HEAP_START_ADDR;
}

void *MemoryAllocator::memAlloc(size_t size) { // size in blocks
    // convert size from blocks to bytes
    size *= MEM_BLOCK_SIZE;
    // find a fitting free memory segment for allocation
    for (MemSegment *currFree = freeMem; currFree != nullptr; currFree = currFree->next) {
        if (currFree->size < size) continue;
        // found first-fitting free memory segment
        if (currFree->size - size <= sizeof(MemSegment)) {
            // no remaining fragment
            if (currFree->prev != nullptr) currFree->prev->next = currFree->next;
            else freeMem = currFree->next;
            if (currFree->next != nullptr) currFree->next->prev = currFree->prev;
            // size remains the same
        } else {
            // link remaining fragment as a new free memory segment
            MemSegment *fragment = (MemSegment *)((uint8 *)currFree + size);
            if (currFree->prev != nullptr) currFree->prev->next = fragment;
            else freeMem = fragment;
            if (currFree->next != nullptr) currFree->next->prev = fragment;
            fragment->next = currFree->next;
            fragment->prev = currFree->prev;
            fragment->size = currFree->size - size;
            // update the size
            currFree->size = size;
        }
        // find location to link newly allocated memory segment
        MemSegment *currAlloc = nullptr;
        if (allocMem == nullptr || currFree < allocMem) currAlloc = nullptr;
        else
            for (currAlloc = allocMem; currAlloc->next != nullptr && currFree > currAlloc->next;
            currAlloc = currAlloc->next);
        // link newly allocated memory segment
        currFree->prev = currAlloc;
        if (currAlloc != nullptr) currFree->next = currAlloc->next;
        else currFree->next = allocMem;
        if (currFree->next != nullptr) currFree->next->prev = currFree;
        if (currFree->prev != nullptr) currFree->prev->next = currFree;
        else allocMem = currFree;
        // skip the header
        return (void *)((uint8 *)currFree + sizeof(MemSegment));
    }
    return nullptr;
}

int MemoryAllocator::memFree(void *addr) {
    // go back to header
    addr = (void *)((uint8 *)addr - sizeof(MemSegment));
    // find given allocated memory segment
    for (MemSegment *currAlloc = allocMem; currAlloc != nullptr; currAlloc = currAlloc->next) {
        if ((void *)currAlloc < addr) continue;
        if ((void *)currAlloc > addr) break;
        // found given allocated memory segment
        if (currAlloc->prev != nullptr) currAlloc->prev->next = currAlloc->next;
        else allocMem = currAlloc->next;
        if (currAlloc->next != nullptr) currAlloc->next->prev = currAlloc->prev;
        // find location to link recently freed memory segment
        MemSegment *currFree = nullptr;
        if (freeMem == nullptr || currAlloc < freeMem) currFree = nullptr;
        else
            for (currFree = freeMem; currFree->next != nullptr && currAlloc > currFree->next;
            currFree = currFree->next);
        // link recently freed memory segment
        currAlloc->prev = currFree;
        if (currFree != nullptr) currAlloc->next = currFree->next;
        else currAlloc->next = freeMem;
        if (currAlloc->next != nullptr) currAlloc->next->prev = currAlloc;
        if (currAlloc->prev != nullptr) currAlloc->prev->next = currAlloc;
        else freeMem = currAlloc;
        // try to merge with next and previous free memory segments
        memTryToJoin(currAlloc);
        memTryToJoin(currAlloc->prev);
        return 0;
    }
    return -1;
}

int MemoryAllocator::memTryToJoin(MemSegment *memSeg) {
    if (memSeg == nullptr) return 0;
    // try to join given memory segment with next memory segment
    if (memSeg->next != nullptr && (uint8 *)memSeg + memSeg->size == (uint8 *)memSeg->next) {
        memSeg->size += memSeg->next->size;
        memSeg->next = memSeg->next->next;
        if (memSeg->next != nullptr) memSeg->next->prev = memSeg;
        return 1;
    } else
        return 0;
}

size_t MemoryAllocator::memGetFreeSpace() {
    size_t freeSpace = 0;
    for (MemSegment *seg = freeMem; seg != nullptr; seg = seg->next) {
        freeSpace += seg->size;
    }
    return freeSpace;
}

size_t MemoryAllocator::memGetLargestFreeBlock() {
    size_t largestFreeBlock = 0;
    for (MemSegment *seg = freeMem; seg != nullptr; seg = seg->next) {
        if (seg->size > largestFreeBlock) largestFreeBlock = seg->size;
    }
    return largestFreeBlock;
}
