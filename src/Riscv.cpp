#include "../h/Riscv.hpp"

#include "../h/CountingSemaphore.hpp"
#include "../h/MemoryAllocator.hpp"
#include "../h/print.hpp"
#include "../h/syscall_codes.hpp"
#include "../h/TCB.hpp"
#include "../lib/console.h"

void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::stopEmulator() {
    uint32 stopValue = 0x5555;
    uint64 stopAddress = 0x100000;
    __asm__ volatile("sw %[stopValue], (%[stopAddress])" : :
        [stopValue] "r"(stopValue), [stopAddress] "r"(stopAddress));
}

void Riscv::handleSupervisorTrap() {
    uint64 scause = r_scause();
    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        // save sepc + 4 [*] to the current thread's stack
        // [*] ecall cause sepc register to be set to the address of ecall instruction itself
        uint64 sepc = r_sepc() + 4;
        // save sstatus register to the current thread's stack
        uint64 sstatus = r_sstatus();
        // retrieve syscall code
        uint64 syscallCode = peek_a0();
        switch (syscallCode) {
            case SYSCALL_CODE_MEM_ALLOC: {
                // retrieve syscall arguments
                size_t size = (size_t)peek_a1();
                // handle syscall
                MemoryAllocator::memAlloc(size);
                // syscall return value in a0
                // pass syscall return value
                // override saved a0 register value on the stack
                uint64 a0 = r_a0();
                replace_a0(a0);
                break;
            }
            case SYSCALL_CODE_MEM_FREE: {
                // retrieve syscall arguments
                void *addr = (void *)r_a1();
                // handle syscall
                MemoryAllocator::memFree(addr);
                // syscall return value in a0
                // pass syscall return value
                // override saved a0 register value on the stack
                uint64 a0 = r_a0();
                replace_a0(a0);
                break;
            }
            case SYSCALL_CODE_MEM_GET_FREE_SPACE: {
                // retrieve syscall arguments
                // (no syscall arguments)
                // handle syscall
                MemoryAllocator::memGetFreeSpace();
                // syscall return value in a0
                // pass syscall return value
                // override saved a0 register value on the stack
                uint64 a0 = r_a0();
                replace_a0(a0);
                break;
            }
            case SYSCALL_CODE_MEM_GET_LARGEST_FREE_BLOCK: {
                // retrieve syscall arguments
                // (no syscall arguments)
                // handle syscall
                MemoryAllocator::memGetLargestFreeBlock();
                // syscall return value in a0
                // pass syscall return value
                // override saved a0 register value on the stack
                uint64 a0 = r_a0();
                replace_a0(a0);
                break;
            }
            case SYSCALL_CODE_THREAD_CREATE: {
                // retrieve syscall arguments
                TCB **handle = (TCB **)peek_a1();
                TCB::StartRoutine startRoutine = (TCB::StartRoutine)peek_a2();
                void *arg = (void *)peek_a3();
                // handle syscall
                *handle = TCB::threadCreate(startRoutine, arg);
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            case SYSCALL_CODE_THREAD_EXIT: {
                // retrieve syscall arguments
                // (no syscall arguments)
                // handle syscall
                TCB::threadExit();
                // syscall return value in a0
                // pass syscall return value
                // override saved a0 register value on the stack
                uint64 a0 = r_a0();
                replace_a0(a0);
                break;
            }
            case SYSCALL_CODE_THREAD_DISPATCH: {
                // retrieve syscall arguments
                // (no syscall arguments)
                // handle syscall
                TCB::dispatch();
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            case SYSCALL_CODE_SEM_OPEN: {
                // retrieve syscall arguments
                CountingSemaphore **handle = (CountingSemaphore **)peek_a1();
                uint32 init = (uint32)peek_a2();
                // handle syscall
                *handle = new CountingSemaphore(init);
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            case SYSCALL_CODE_SEM_CLOSE: {
                // retrieve syscall arguments
                CountingSemaphore *handle = (CountingSemaphore *)peek_a1();
                // handle syscall
                delete handle;
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            case SYSCALL_CODE_SEM_WAIT: {
                // retrieve syscall arguments
                CountingSemaphore *id = (CountingSemaphore *)peek_a1();
                // handle syscall
                id->semWait();
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            case SYSCALL_CODE_SEM_SIGNAL: {
                // retrieve syscall arguments
                CountingSemaphore *id = (CountingSemaphore *)peek_a1();
                // handle syscall
                id->semSignal();
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            case SYSCALL_CODE_GETC: {
                // retrieve syscall arguments
                // (no syscall arguments)
                // handle syscall
                __getc();
                // syscall return value in a0
                // pass syscall return value
                // override saved a0 register value on the stack
                uint64 a0 = r_a0();
                replace_a0(a0);
                break;
            }
            case SYSCALL_CODE_PUTC: {
                // retrieve syscall arguments
                char c = (char)peek_a1();
                // handle syscall
                __putc(c);
                // pass syscall return value
                // (no syscall return value)
                break;
            }
            default: {
                // unexpected syscall code
                printErrorDetails();
                stopEmulator();
                break;
            }
        }
        // restore sstatus register from the current thread's stack
        w_sstatus(sstatus);
        // restore sepc register from the current thread's stack
        w_sepc(sepc);
    } else if (scause == 0x8000000000000001UL) {
        // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
        mc_sip(SIP_SSIP);
        TCB::timeSliceCounter++;
        if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
            uint64 volatile sepc = r_sepc();
            uint64 volatile sstatus = r_sstatus();
            TCB::timeSliceCounter = 0;
            TCB::dispatch();
            w_sstatus(sstatus);
            w_sepc(sepc);
        }
    } else if (scause == 0x8000000000000009UL) {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    } else {
        // unexpected trap cause
        printErrorDetails();
        stopEmulator();
    }
}

void Riscv::printErrorDetails() {
    printCharConstArray("\n");
    // print scause
    uint64 scause = r_scause();
    printCharConstArray("scause ");
    printValueHexadecimal(scause);
    // print sepc
    uint64 sepc = r_sepc();
    printCharConstArray("\nsepc=");
    printValueHexadecimal(sepc);
    // print stval
    uint64 stval = r_stval();
    printCharConstArray(" stval=");
    printValueHexadecimal(stval);
    // print panic
    printCharConstArray("\npanic: kerneltrap\n");
}
