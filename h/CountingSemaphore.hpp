#ifndef COUNTINGSEMAPHORE_HPP
#define COUNTINGSEMAPHORE_HPP

#include "List.hpp"
#include "TCB.hpp"
#include "../lib/hw.h"

class CountingSemaphore {
public:
    CountingSemaphore(uint32 init) : value(init) {}

    virtual ~CountingSemaphore();

    void semWait();

    virtual void semSignal();

protected:
    int value;

    void block();

    void deblock();

private:
    friend class RISCV;

    List<TCB> blockedThreadQueue;
};

#endif // COUNTINGSEMAPHORE_HPP
