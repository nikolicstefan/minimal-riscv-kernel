#include "../h/MemoryAllocator.hpp"
#include "../h/syscall_c.h"
#include "../lib/hw.h"

using size_t = decltype(sizeof(0));

void *operator new(size_t size) {
    // size += sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1;
    // size /= MEM_BLOCK_SIZE;
    // return MemoryAllocator::memAlloc(size);
    return mem_alloc(size);
}

void *operator new[](size_t size) {
    // size += sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1;
    // size /= MEM_BLOCK_SIZE;
    // return MemoryAllocator::memAlloc(size);
    return mem_alloc(size);
}

void operator delete(void *addr) noexcept {
    // MemoryAllocator::memFree(addr);
    mem_free(addr);
}

void operator delete[](void *addr) noexcept {
    // MemoryAllocator::memFree(addr);
    mem_free(addr);
}
