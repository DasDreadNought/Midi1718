#ifndef ENDIANNESS_H
#define ENDIANNESS_H


#include <stdint.h>
#include <algorithm>


void switch_endianness(uint16_t* x);
void switch_endianness(uint32_t* x);

#endif // !ENDIANNESS_H
