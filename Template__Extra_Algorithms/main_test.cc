// C++ headers
#include <vector>
#include <iostream>

// own header
#include "Template_count_adjacent_if.h"

int main()
{
#if 0 // Test 1
	std::vector<int> vec{ 1,1,2,4,5,5,6,6 };
	auto BinPred = [](const auto& lhs, const auto& rhs) { return lhs + rhs == 3 ; };
	std::cout << ::count_adjacent_if(vec.begin(), vec.end()) <<'\n';
	std::cout << ::count_adjacent_if2(vec.cbegin(), vec.cend(), BinPred) <<'\n';
#endif
	return 0;
}
