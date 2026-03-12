#include "../h/TCB.hpp"

#include "../h/Riscv.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB *TCB::threadCreate(StartRoutine startRoutine, void *arg) {
    return new TCB(startRoutine, arg);
}

int TCB::threadExit() {
    if (running != nullptr) {
        running->setFinished(true);
        dispatch();
        return 0; // unreachable
    }
    return -1;
}

void TCB::yield() {
    Riscv::w_a0(0x13);
    __asm__ volatile ("ecall");
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->startRoutine(running->arg);
    running->setFinished(true);
    dispatch();
}

void *TCB::operator new(size_t size) {
    size += sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1;
    size /= MEM_BLOCK_SIZE;
    return MemoryAllocator::memAlloc(size);
}

void *TCB::operator new[](size_t size) {
    size += sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1;
    size /= MEM_BLOCK_SIZE;
    return MemoryAllocator::memAlloc(size);
}

void TCB::operator delete(void *addr) noexcept {
    MemoryAllocator::memFree(addr);
}

void TCB::operator delete[](void *addr) noexcept {
    MemoryAllocator::memFree(addr);
}
