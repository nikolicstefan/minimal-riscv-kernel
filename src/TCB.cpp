#include "../h/TCB.hpp"

#include "../h/print.hpp"
#include "../h/Riscv.hpp"

TCB *TCB::running = nullptr;

uint64 TCB::timeSliceCounter = 0;

TCB::TCB(StartRoutine startRoutine, void *arg) :
    startRoutine(startRoutine),
    arg(arg),
    unalignedStack(nullptr),
    stack(nullptr),
    context({0, 0}),
    timeSlice(DEFAULT_TIME_SLICE),
    sleepInterval(0),
    finished(false) {
    if (startRoutine != nullptr) {
        uint64 stackSize = DEFAULT_STACK_SIZE * sizeof(uint64) + 15;
        stackSize += sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1;
        stackSize /= MEM_BLOCK_SIZE;
        unalignedStack = (uint64 *)MemoryAllocator::memAlloc(stackSize);
        stack = (uint64 *)((uint64)((uint8 *)unalignedStack + 15) & ~0xFUL);
        context.ra = (uint64)&threadWrapper;
        context.sp = (uint64)&stack[DEFAULT_STACK_SIZE];
        Scheduler::put(this);
    }
}

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

void TCB::timeSleep(uint64 sleepInterval) {
    if (sleepInterval == 0) return;
    running->setSleepInterval(sleepInterval);
    TCB *old = running;
    Scheduler::suspend(old);
    running = Scheduler::get();
    contextSwitch(&old->context, &running->context);
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
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    Riscv::popSppSpie();
    // U-mode
    running->startRoutine(running->arg);
    running->setFinished(true);
    yield();
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
