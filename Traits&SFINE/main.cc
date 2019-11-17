// C++ headers
#include <iostream>

// own header
#include "SFINAE.hpp"
#include "Traits.hpp"

#include <vector>

class MyClass
{
	using vec = std::vector<int>;
};

int main()
{
	
#if 0
	std::cout << Function(nullptr)	<< " ";
	std::cout << Function(2)		<< " ";
	std::cout << Function(2.0f)		<< " ";
	std::cout << Function((2.0))	<< "\n";
#endif 

#if 1
	class_type<decltype(&MyStruct::foo)> obj{};
	ret_type<decltype(&MyStruct::foo)> a{ obj.foo(1) };
#endif
	return  0;
}
