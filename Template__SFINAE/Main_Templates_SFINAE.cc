// C++ headers
#include <iostream>
// own header
#include "Templates_SFINAE.h"

int main()
{
	std::cout << Function(nullptr)	<< " ";
	std::cout << Function(2)		<< " ";
	std::cout << Function(2.0f)		<< " ";
	std::cout << Function((2.0))	<< "\n";

	return  0;
}
