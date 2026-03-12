#include "../h/syscall_c.h"

#include "../h/MemoryAllocator.hpp"
#include "../h/Riscv.hpp"
#include "../h/syscall_codes.hpp"

static void syscall(uint64 syscallCode, ...) {
    // uint64 syscallCode in a0
    // ... (syscall arguments) in a1, ..., a7
    __asm__ volatile ("ecall");
}

void *mem_alloc(size_t size) { // size in bytes
    // convert size from bytes to blocks
    size = (size + sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    syscall(SYSCALL_CODE_MEM_ALLOC, size);
    // syscall return value in a0
    // retrieve syscall return value
    uint64 retVal = Riscv::r_a0();
    return (void *)retVal;
}

int mem_free(void *addr) {
    syscall(SYSCALL_CODE_MEM_FREE, addr);
    // syscall return value in a0
    // retrieve syscall return value
    uint64 retVal = Riscv::r_a0();
    return (int)retVal;
}

size_t mem_get_free_space() {
    syscall(SYSCALL_CODE_MEM_GET_FREE_SPACE);
    // syscall return value in a0
    // retrieve syscall return value
    uint64 retVal = Riscv::r_a0();
    return (size_t)retVal;
}

size_t mem_get_largest_free_block() {
    syscall(SYSCALL_CODE_MEM_GET_LARGEST_FREE_BLOCK);
    // syscall return value in a0
    // retrieve syscall return value
    uint64 retVal = Riscv::r_a0();
    return (size_t)retVal;
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    syscall(SYSCALL_CODE_THREAD_CREATE, handle, start_routine, arg);
    // retrieve syscall return value
    // (no syscall return value)
    if (*handle == nullptr) return -1;
    else return 0;
}

int thread_exit() {
    syscall(SYSCALL_CODE_THREAD_EXIT);
    // syscall return value in a0
    // retrieve syscall return value
    uint64 retVal = Riscv::r_a0();
    return (int)retVal;
}

void thread_dispatch() {
    syscall(SYSCALL_CODE_THREAD_DISPATCH);
    // retrieve syscall return value
    // (no syscall return value)
}
