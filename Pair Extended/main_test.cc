// C++ headers
#include <iostream>
#include <numeric>
#include <vector>

// own header
#include "PairExtT.h"

using Type = Pair<double, double>;

int main()
{
	std::vector<Type> vec2(5, std::make_pair(0.1, 1.1));
	for (const auto& pair : vec2)  std::cout << pair;
	Type p = std::make_pair(0.1, 1.1);
	std::iota(vec2.begin(), vec2.end(), p); std::cout << "\n";
	for (const auto& pair : vec2)  std::cout << pair;
	return 0;
}