#include "../h/MemoryAllocator.hpp"
#include "../h/print.hpp"
#include "../h/Riscv.hpp"
#include "../h/TCB.hpp"
#include "../h/workers_cpp.hpp"

int main() {
    MemoryAllocator::memInit();
    Riscv::w_stvec((uint64) &Riscv::supervisorTrapVector | 0x1);

    TCB *mainThread = TCB::threadCreate(nullptr, nullptr);
    TCB::running = mainThread;

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    TCB *workersThread = TCB::threadCreate(workersCPP, nullptr);

    while (!workersThread->isFinished()) {
        TCB::yield();
    }

    delete mainThread;
    delete workersThread;

    printString("Finished\n");
    Riscv::stopEmulator();

    return 0;
}
