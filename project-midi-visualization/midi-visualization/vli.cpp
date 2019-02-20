#include "vli.h"

uint32_t read_variable_length_integer(std::istream& in) {

	uint32_t temp = 0x0000;

	char c;
	while (in.get(c)) {
		temp |= (c & 0x7f);
		if (!(c & 0x80)) {
			break;
		}
		else {
			temp <<= 7;
		}
	}


	return temp;


}