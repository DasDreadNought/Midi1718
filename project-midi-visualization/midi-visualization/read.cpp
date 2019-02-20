#include "read.h"

uint8_t read_byte(std::istream& in) {
	uint8_t b = 0x00;
	read(in, &b);
	return b;
}
