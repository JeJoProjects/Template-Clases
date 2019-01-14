// C++ headers
#include <vector>
#include <iostream>

// own header
#include "Template_count_adjacent_if.h"
#include "Template_duplicates.h"

int main()
{
#if 1 // Test 1: count_adjacent_if
	std::cout << "Test 1: count_adjacent_if...\n";
	std::vector<int> vec{ 1,1,2,4,5,5,6,6 };
	auto BinPred = [](const auto& lhs, const auto& rhs) { return lhs + rhs == 3 ; };
	std::cout << ::count_adjacent_if(vec.begin(), vec.end()) <<'\n';
	// std::cout << ::count_adjacent_if2(vec.cbegin(), vec.cend(), BinPred) <<'\n'; // error
#endif

#if 1 // Test 2: has_duplicates
	std::cout << "Test 2: has_duplicates...\n";
	std::vector<char> vec0 = { 'c','a','i','o','p','a' };
	std::cout << std::boolalpha << ::has_duplicates(vec0.cbegin(), vec0.cend()) << '\n';

	std::vector<int> vec1 = { 1, 2, 8, 1, 2, 7, 2 };
	std::cout << std::boolalpha << ::has_duplicates(vec1.begin(), vec1.end()) << '\n';

	std::vector<std::string> vec2 = { "Je", "Jo", "Ji", "Jy", "Ja", "Jc", "Jq" };
	std::cout << std::boolalpha << ::has_duplicates(vec2.cbegin(), vec2.cend()) << '\n';
#endif
	return 0;
}
