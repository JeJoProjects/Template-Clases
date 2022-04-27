// C++ headers
#include <string_view>
using namespace std::string_view_literals;

// own header
#include "TestFunctions.hpp"

using namespace JeJo;

int main()
{
	
#if 0 // Traits and SFINAE test
	std::cout << "SFINAE test!...\n";
	sfinae_test();

#elif 0 // callMemsForAllObjects() test
	std::cout << "\n\ncallMemsForAllObjects() test!...\n";
	callMemsForAllObjects(
		std::make_tuple(&JeJo::MyClass::printData1, 200),
		std::make_tuple(&JeJo::MyClass::printData2, 201, 50.55),
		std::make_tuple(&JeJo::MyClass::printData3, "JeJo")
		);

	// iterate forward and backwards!!!!
	print_args(1, 2.4f, 'd', "const char[15]", "string"s);

#elif 1
	// class trait test
	const traits::ClassTraitTest traitsTest{ "Test: class_traits"sv };
	traitsTest.test();

	// is_specialization_of trait/ concept test
	const traits::IsSpecializationOfTest isSpecOfTest{
		"Test: is_specialization_of trait/ concept test"sv
	};
	isSpecOfTest.test1();
	isSpecOfTest.test2();

	// has member function sfinae test
	traits::has_member_function{}.test();

#endif
	return  0;
}
