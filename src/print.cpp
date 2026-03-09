#include "../h/print.hpp"
#include "../h/Riscv.hpp"
#include "../lib/console.h"

void printString(char const *string) {
    // uint64 sstatus = Riscv::r_sstatus();
    // Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

    while (*string != '\0') {
        __putc(*string);
        string++;
    }

    // Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

void printValueDecimal(uint64 value) {
    // uint64 sstatus = Riscv::r_sstatus();
    // Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

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

    // Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE ? Riscv::SSTATUS_SIE : 0);
}

void printValueHexadecimal(uint64 value) {
    // uint64 sstatus = Riscv::r_sstatus();
    // Riscv::mc_sstatus(Riscv::SSTATUS_SIE);

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

    // Riscv::ms_sstatus(sstatus & Riscv::SSTATUS_SIE? Riscv::SSTATUS_SIE : 0);
}