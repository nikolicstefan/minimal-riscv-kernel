#ifndef PRINT_HPP
#define PRINT_HPP

#include "../lib/hw.h"

extern "C" uint64 copy_and_swap(uint64 &lock, uint64 expected, uint64 desired);

extern void printString(char const *string);

extern void printValueDecimal(uint64 value);

extern void printValueHexadecimal(uint64 value);

#endif // PRINT_HPP
