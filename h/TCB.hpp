#ifndef TCB_HPP
#define TCB_HPP

#include "MemoryAllocator.hpp"
#include "Scheduler.hpp"
#include "../lib/hw.h"

class TCB {
public:
    using StartRoutine = void (*)(void *);

    static TCB *running;

    ~TCB() { MemoryAllocator::memFree(unalignedStack); }

    static TCB *threadCreate(StartRoutine startRoutine, void *arg);

    static int threadExit();

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
    friend class CountingSemaphore;

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    static uint64 timeSliceCounter;

    StartRoutine startRoutine;
    void *arg;
    uint64 *unalignedStack;
    uint64 *stack;
    Context context;
    uint64 timeSlice;
    bool finished;

    TCB(StartRoutine startRoutine, void *arg) :
    startRoutine(startRoutine),
    arg(arg),
    unalignedStack(nullptr),
    stack(nullptr),
    context({0, 0}),
    timeSlice(DEFAULT_TIME_SLICE),
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

    static void threadWrapper();

    static void dispatch();

    static void contextSwitch(Context *oldContext, Context *runningContext);
};

#endif // TCB_HPP
