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

    static void timeSleep(uint64 sleepInterval);

    static void yield();

    bool isFinished() const { return finished; }

    void setFinished(bool value) { finished = value; }

    uint64 getTimeSlice() const { return timeSlice; }

    uint64 getSleepInterval() const { return sleepInterval; }

    void setSleepInterval(uint64 value) { sleepInterval = value; }

    void decreaseSleepInterval(uint64 value) { sleepInterval -= value; }

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
    uint64 sleepInterval;
    bool finished;

    TCB(StartRoutine startRoutine, void *arg);

    static void threadWrapper();

    static void dispatch();

    static void contextSwitch(Context *oldContext, Context *runningContext);
};

#endif // TCB_HPP
