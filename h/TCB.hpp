#ifndef TCB_HPP
#define TCB_HPP

#include "MemoryAllocator.hpp"
#include "Scheduler.hpp"
#include "../lib/hw.h"

class TCB {
public:
    using Body = void (*)();

    static TCB *running;

    ~TCB() { MemoryAllocator::memFree(unalignedStack); }

    static TCB *createThread(Body body);

    static void yield();

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    void *operator new(size_t size);

    void *operator new[](size_t size);

    void operator delete(void *addr) noexcept;

    void operator delete[](void *addr) noexcept;

private:
    friend class Riscv;

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    static uint64 timeSliceCounter;

    Body body;
    uint64 *unalignedStack;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    TCB(Body body, uint64 timeSlice) :
    body(body),
    unalignedStack(nullptr),
    stack(nullptr),
    context({(uint64)&threadWrapper, 0}),
    timeSlice(timeSlice),
    finished(false) {
        if (body != nullptr) {
            uint64 stackSize = DEFAULT_STACK_SIZE * sizeof(uint64) + 15;
            stackSize += sizeof(MemoryAllocator::MemSegment) + MEM_BLOCK_SIZE - 1;
            stackSize /= MEM_BLOCK_SIZE;
            unalignedStack = (uint64 *)MemoryAllocator::memAlloc(stackSize);
            stack = (uint64 *)((uint64)((uint8 *)unalignedStack + 15) & ~0xFUL);
            context.sp = (uint64)&stack[DEFAULT_STACK_SIZE];
            Scheduler::put(this);
        }
    }

    static void threadWrapper();

    static void dispatch();

    static void contextSwitch(Context *oldContext, Context *runningContext);
};

#endif // TCB_HPP
