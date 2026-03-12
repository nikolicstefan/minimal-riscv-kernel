#include "../h/CountingSemaphore.hpp"

CountingSemaphore::~CountingSemaphore() {
    while (!blockedThreadQueue.isEmpty())
        deblock();
}

void CountingSemaphore::semWait() {
    if (--this->value < 0)
        this->block();
}

void CountingSemaphore::block() {
    TCB::timeSliceCounter = 0;
    TCB *old = TCB::running;
    if (!old->isFinished()) this->blockedThreadQueue.addLast(old);
    TCB::running = Scheduler::get();
    TCB::contextSwitch(&old->context, &TCB::running->context);
}

void CountingSemaphore::semSignal() {
    if (this->value++ < 0)
        this->deblock();
}

void CountingSemaphore::deblock() {
    Scheduler::put(this->blockedThreadQueue.removeFirst());
}
