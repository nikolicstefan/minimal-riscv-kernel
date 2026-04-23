# minimal-riscv-kernel

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

A minimal preemptive multi-threaded kernel for the RISC-V architecture (RV64IMA), implemented in C++ and RISC-V assembly. The kernel runs as a statically linked library-style kernel alongside a user application in a single shared address space, targeting embedded-style environments without dynamic loading.

*Course project for Operating Systems 1 at the School of Electrical Engineering, University of Belgrade (ETF), 2024/2025.*

---

## Target Platform

**Emulated hardware**

| Property | Value |
|---|---|
| ISA | RISC-V RV64IMA |
| Memory | 128 MB |
| CPU cores | 1 |
| Timer frequency | 10 Hz (100 ms period) |
| Privilege levels | U-mode (user), S-mode (supervisor) |

**Host / build environment**

| Property | Value |
|---|---|
| Host OS | Stripped xv6 - single process, full physical address space |
| C++ standard | C++11 |
| Compiler | `riscv64-linux-gnu-g++` |
| Emulator | `qemu-system-riscv64` |

---

## Features

- **Memory allocator** - first-fit heap allocator over a fixed physical memory region (`HEAP_START_ADDR` to `HEAP_END_ADDR`)
- **Thread management** - creation, dispatch, and exit of user-space threads with per-thread stacks
- **Semaphores** - counting semaphores with blocking `wait` / `signal` semantics
- **Preemptive scheduling** - FIFO round-robin scheduler with time-slice preemption driven by a hardware timer interrupt
- **Asynchronous context switch** - context preemption on timer and keyboard (UART) interrupts
- **`time_sleep`** - timer-based thread suspension using a delta-list of sleeping threads
- **Console I/O** - buffered, interrupt-driven `getc` / `putc` via a UART serial controller *(not yet implemented)*
- **Three-layer kernel interface** - ABI (via `ecall`), C API, and C++ OO API

---

## Architecture

The kernel is structured as a monolithic, single-address-space library kernel:

```
User application (app.lib)
        │
   C++ OO API          (syscall_cpp.hpp)
        │
    C API              (syscall_c.hpp)
        │
    ABI layer          (ecall / trap handler)
        │
    Kernel core        (C++ + RISC-V assembly)
        │
 HW access module      (hw.lib)
```

All layers are statically linked into a single executable. Kernel code runs in **supervisor (S-mode)** while user code runs in **user (U-mode)**. Trap handling uses vectored mode (`stvec` with `MODE=1`), with all slots in the vector table pointing to the same handler routine - which branches on `scause` to handle system calls (`ecall`), timer interrupts, external hardware interrupts, and exceptions.

## Key Abstractions in Kernel Core

| Class | Responsibility |
|---|---|
| `MemoryAllocator` | Heap allocator (singleton) |
| `Thread` / `PCB` | Thread state, context, stack |
| `Scheduler` | Ready queue and scheduling policy |
| `Semaphore` | Counting semaphore with blocked-thread queue |
| `Console` | Buffered UART I/O with kernel I/O threads |

---

## System Call Interface (ABI)

System calls are issued via the `ecall` instruction, with the following codes:

| Code | Function |
|---|---|
| `0x01` | `mem_alloc(size)` |
| `0x02` | `mem_free(ptr)` |
| `0x11` | `thread_create(handle, routine, arg, stack)` |
| `0x12` | `thread_exit()` |
| `0x13` | `thread_dispatch()` |
| `0x21` | `sem_open(handle, init)` |
| `0x22` | `sem_close(handle)` |
| `0x23` | `sem_wait(id)` |
| `0x24` | `sem_signal(id)` |
| `0x31` | `time_sleep(ticks)` |
| `0x41` | `getc()` |
| `0x42` | `putc(char)` |

---

## Repository Structure

```
.
├── h/                  # Header files (kernel internals + public API)
├── src/                # Kernel source files (.cpp and .S)
├── lib/                # Pre-built static libraries (hw.lib, mem.lib, console.lib)
├── test/               # User application test cases
├── Makefile
├── kernel.ld           # Linker script
└── .gdbinit.tmpl-riscv # GDB init template for remote debugging
```

---

## Build & Run

### Prerequisites

- `riscv64-linux-gnu-gcc` (cross-compiler toolchain)
- `qemu-system-riscv64`
- `make`
- CLion (optional, recommended IDE) or any terminal

### Build and run in QEMU

```bash
make qemu
```

### Build and run with GDB debugging

```bash
make qemu-gdb
```

Then attach `gdb-multiarch` from a second terminal (or from CLion's remote debug configuration):

```
target remote localhost:<PORT>
```

The port is dynamically generated and printed by the `qemu-gdb` target at startup.

### Clean build artifacts

```bash
make clean
```

---

## Tests

The test suite is interactive - on startup, the kernel prompts for a test number (`[1-7]`). Each test is gated by a compile-time flag (`LEVEL_2/3/4_IMPLEMENTED`) corresponding to the project task it covers.

| # | File | Task | Description |
|---|---|---|---|
| 1 | `Threads_C_API_test` | Task 2 | Basic thread lifecycle using the C API - creates multiple threads, verifies correct creation, dispatch, and exit with synchronous context switching only. |
| 2 | `Threads_CPP_API_test` | Task 2 | Same as Test 1 but exercises the C++ OO API (`Thread` class), verifying that the C++ wrapper layer correctly delegates to the underlying C API. |
| 3 | `ConsumerProducer_C_API_test` | Task 3 | Classic producer-consumer problem implemented with the C API semaphores (`sem_open`, `sem_wait`, `sem_signal`, `sem_close`) and synchronous context switching. |
| 4 | `ConsumerProducer_CPP_Sync_API_test` | Task 3 | Same producer-consumer scenario using the C++ `Semaphore` class, verifying the full C++ API with synchronous context switching. |
| 5 | `ThreadSleep_C_API_test` | Task 4 | Tests `time_sleep` via the C API - spawns threads that sleep for varying durations and verifies they wake up in the correct order relative to the timer. |
| 6 | `ConsumerProducer_CPP_API_test` | Task 4 | Producer-consumer with the C++ API under **asynchronous** context switching - verifies correct behaviour when preemption can occur at any point, including during kernel execution. |
| 7 | `System_Mode_test` | Task 2 | Verifies that user-mode threads execute in **U-mode** (unprivileged). The test intentionally triggers a privileged operation from user code and expects the kernel to catch the resulting exception - if the test completes without a trap, the privilege separation is broken. |

---

## License

The host environment (xv6) is:

Copyright (c) 2006-2019 Frans Kaashoek, Robert Morris, Russ Cox - MIT

Modifications to xv6:

Copyright (c) 2022 Zivojin Sustran, University of Belgrade

Kernel implementation:

Copyright (c) 2026 Stefan Nikolic

All components are released under the MIT License. See [LICENSE](LICENSE) for full text.

