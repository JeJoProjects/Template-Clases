// C++ headers
#include <string_view>
using namespace std::string_view_literals;

// own header
#include "TestFunctions.hpp"



int main()
{
	
#if 1 // Traits and SFINAE test
	std::cout << "SFINAE test!...\n";
	sfinae_test();
#endif


#if 1 // callMemsForAllObjects() test
	std::cout << "\n\nCallMemsForAllObjects() test!...\n";
	callMemsForAllObjects_test();

	// iterate forward and backwards using lambda technique... print_args()
	std::cout << "\n\nMagic print_args() Lambda test!...\n";
	//print_args_test();


	// Sequence container stringify() test
	std::cout << "\n\nStringing the containers of any dimensions!...\n";
	void stringify_test();

#endif

#if 0
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
