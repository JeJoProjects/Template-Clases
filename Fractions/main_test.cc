// C++ headers
#include <iostream>
#include <vector>
#include <cstdint>

// own header
#include "FractionT.h"

// test code
int main()
{
	Fraction<int> obj( 6, 8 );
	std::cout << obj << " " << obj.getReal() << std::endl;

	// @todo: implimentation for float and doubles
	// Fraction<float> obj1(3.5f, 1.0f);
	// std::cout << obj1 << " " << obj1.getReal() << std::endl;
	return 0;
}
