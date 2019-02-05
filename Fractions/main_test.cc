// C++ headers
#include <iostream>
#include <vector>
#include <cstdint>

// own header
#include "FractionT.h"

// test code
int main()
{
	for (int i = 0; i <= 80; ++i)
	{
		Fraction<int> obj(i, 8);
		std::cout << obj << " " << obj.getReal() << std::endl;
	}

	// @todo: implementation for float and doubles
	// Fraction<float> obj1(3.5f, 1.0f);
	// std::cout << obj1 << " " << obj1.getReal() << std::endl;
	return 0;
}
