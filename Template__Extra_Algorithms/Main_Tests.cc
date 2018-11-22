/* count_adjacent_if
 *
 */

#include <vector>
#include <iostream>
#include <algorithm>
#include "Template_count_adjacent_if.h"

int main()
{
	std::vector<int> vec{ 1,1,2,4,5,5,6,6 };
	auto BinPred = [](const auto& lhs, const auto& rhs) { return lhs == rhs; };
	std::cout << extra::count_adjacent_if(vec.begin(), vec.end()) << std::endl;
	return 0;
}
