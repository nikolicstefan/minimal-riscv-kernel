#include "../h/MemoryAllocator.hpp"
#include "../h/print.hpp"
#include "../h/Riscv.hpp"
#include "../h/TCB.hpp"

void userMain();

void userMainWrapper(void *) {
    userMain();
}

int main() {
    MemoryAllocator::memInit();
    Riscv::w_stvec((uint64)&Riscv::supervisorTrapVector | 0x1);

    TCB *threads[2];

    threads[0] = TCB::threadCreate(nullptr, nullptr);
    TCB::running = threads[0];

    Riscv::ms_sstatus(Riscv::SSTATUS_SIE);

    threads[1] = TCB::threadCreate(&userMainWrapper, nullptr);

    while (!threads[1]->isFinished()) {
        TCB::yield();
    }

    for (TCB* &thread: threads) {
        delete thread;
    }

    printCharConstArray("Finished\n");
    Riscv::stopEmulator();

    return 0;
}
