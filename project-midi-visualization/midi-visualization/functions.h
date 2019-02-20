#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>

template <typename T>
bool read(std::istream& in, T* x) {
	in.read(reinterpret_cast<char*>(x), sizeof(*x));
	return !in.eof();
}

#endif // !FUNCTIONS_H
