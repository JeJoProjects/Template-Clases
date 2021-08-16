// C++ headers
#include <iostream>
#include <string>
#include <string_view>
using namespace std::literals;
using namespace std::string_literals;

// own header
#include "SFINAE.hpp"
#include "Traits.hpp"
#include "TemplateFunctions.hpp"

struct A
{
	std::vector<int> vec() { return {}; }
};

struct B
{
	std::vector<double> vec() { return {}; }
};


int main()
{
	
#if 1 // Traits and SFINAE test
	std::cout << "SFINAE test!...\n";
	std::cout << JeJo::Function(nullptr)	<< " ";
	std::cout << JeJo::Function(2)		<< " ";
	std::cout << JeJo::Function(2.0f)		<< " ";
	std::cout << JeJo::Function((2.0))	<< "\n";
#endif 


#if 1
	static_assert(HasVector<A>);
	static_assert(HasVector<B>);
#endif

#if 1  // callMemsForAllObjects() test
	std::cout << "\n\ncallMemsForAllObjects() test!...\n";
	JeJo::callMemsForAllObjects(
		std::make_tuple(&JeJo::MyClass::printData1, 200),
		std::make_tuple(&JeJo::MyClass::printData2, 201, 50.55),
		std::make_tuple(&JeJo::MyClass::printData3, "JeJo")
		);
#endif

	// class trait test
	const JeJo::ClassTraitTest traitsTest{ "Test: class_traits"sv };
	traitsTest.test();
	return  0;
}
