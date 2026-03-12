#include "../h/print.hpp"

#include "../h/Riscv.hpp"
#include "../h/syscall_c.h"
#include "../lib/console.h"

uint64 lockPrint = 0;

#define LOCK() while(copy_and_swap(lockPrint, 0, 1)) thread_dispatch()
#define UNLOCK() while(copy_and_swap(lockPrint, 1, 0))

void printString(char const *string) {
    LOCK();

    while (*string != '\0') {
        __putc(*string);
        string++;
    }

    UNLOCK();
}

void printValueDecimal(uint64 value) {
    LOCK();

    static char digits[] = "0123456789";
    char buf[16];
    int i, neg;
    uint x;

    neg = 0;
    if (value < 0) {
        neg = 1;
        x = -value;
    } else {
        x = value;
    }

    i = 0;
    do {
        buf[i++] = digits[x % 10];
    } while ((x /= 10) != 0);
    if (neg)
        buf[i++] = '-';

    while (--i >= 0) { __putc(buf[i]); }

    UNLOCK();
}

void printValueHexadecimal(uint64 value) {
    LOCK();

    static char digits[] = "0123456789abcdef";
    char buf[18];
    int i;

    i = 0;
    while (i < 16) {
        buf[i++] = digits[value % 16];
        value /= 16;
    }

    buf[i++] = 'x';
    buf[i++] = '0';

    while (--i >= 0)
        __putc(buf[i]);

    UNLOCK();
}
