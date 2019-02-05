// C++ headers
#include <iostream>
#include <vector>
#include <cstdint>

// own header
#include "FractionT.h"

// test code
int main()
{
	Fraction obj{ 6, 8 };
	std::cout << obj << " " << obj.getReal() << std::endl;
	return 0;
}
