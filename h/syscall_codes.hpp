#ifndef SYSCALL_CODES_HPP
#define SYSCALL_CODES_HPP

#include "../lib/hw.h"

static const uint64 SYSCALL_CODE_MEM_ALLOC = 0x01;
static const uint64 SYSCALL_CODE_MEM_FREE = 0x02;
static const uint64 SYSCALL_CODE_MEM_GET_FREE_SPACE = 0x03;
static const uint64 SYSCALL_CODE_MEM_GET_LARGEST_FREE_BLOCK = 0x04;
static const uint64 SYSCALL_CODE_THREAD_CREATE = 0x11;
static const uint64 SYSCALL_CODE_THREAD_EXIT = 0x12;
static const uint64 SYSCALL_CODE_THREAD_DISPATCH = 0x13;
static const uint64 SYSCALL_CODE_SEM_OPEN = 0x21;
static const uint64 SYSCALL_CODE_SEM_CLOSE = 0x22;
static const uint64 SYSCALL_CODE_SEM_WAIT = 0x23;
static const uint64 SYSCALL_CODE_SEM_SIGNAL = 0x24;
static const uint64 SYSCALL_CODE_TIME_SLEEP = 0x31;
static const uint64 SYSCALL_CODE_GETC = 0x41;
static const uint64 SYSCALL_CODE_PUTC = 0x42;

#endif // SYSCALL_CODES_HPP
