// C++ headers
#include <iostream>

// own header
#include "SFINAE.hpp"
#include "Traits.hpp"

int main()
{
#if 0
	std::cout << Function(nullptr)	<< " ";
	std::cout << Function(2)		<< " ";
	std::cout << Function(2.0f)		<< " ";
	std::cout << Function((2.0))	<< "\n";
#endif 

#if 1
	get_class_t<decltype(&MyStruct::foo)> obj{};
	obj.foo();

#endif
	return  0;
}
