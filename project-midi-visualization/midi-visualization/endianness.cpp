#include "endianness.h"

void switch_endianness(uint16_t* x) {
	// One-liner of everything
	*x = (((0xFF & *x) << 8) | (0xFF & (*x >> 8)));

	/*
	This is testing everyting separately

	uint8_t a = 0xFF & (*x >> 0);
	uint8_t b = 0xFF & (*x >> 8);

	uint16_t c = a << 8;
	uint16_t y = c | b;


	std::cout << "a: " << std::bitset<8>(a)
	<< ", b: " << std::bitset<8>(b)
	<< ", c: " << std::bitset<16>(c)
	<< ", (x: " << std::bitset<16>(*x) << ")"
	<< ", (y: " << std::bitset<16>(y) << ")" << std::endl;

	std::cout << "x: " << *x
	<< ", y: " << y << std::endl;



	std::cout << "-------------------------------------" << std::endl;


	*x = y;*/

}

void switch_endianness(uint32_t* x) {

	// One-liner of everything
	std::reverse(reinterpret_cast<uint8_t*>(x), reinterpret_cast<uint8_t*>(x) + 4);

	/*
	This is testing everyting separately
	uint8_t* y = reinterpret_cast<uint8_t*>(x);


	std::cout << "x: " << std::bitset<32>(*x) << std::endl;
	std::cout << "y: [ " << std::bitset<8>(y[0])
	<< " , " << std::bitset<8>(y[1])
	<< " , " << std::bitset<8>(y[2])
	<< " , " << std::bitset<8>(y[3])

	<< " ]" << std::endl;

	std::reverse(y, y + 4);

	std::cout << "Reverse y: [ " << std::bitset<8>(y[0])
	<< " , " << std::bitset<8>(y[1])
	<< " , " << std::bitset<8>(y[2])
	<< " , " << std::bitset<8>(y[3])

	<< " ]" << std::endl;

	uint32_t* z = reinterpret_cast<uint32_t*>(y);
	std::cout << "z: " << std::bitset<32>(*z) << std::endl;

	*/
}
