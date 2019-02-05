// C++ headers
#include <iostream>
#include <vector>
#include <cstdint>

// own header
#include "VectorExtendedT.h"

// test code
int main()
{
	VectorExt<int> obj(10);
	std::cout << std::make_pair(obj, Mode::new_line);
	std::cout << std::make_pair(obj, Mode::space);

	VectorExt<int64_t> Obj1 = { 1,8,2,3,4,7,5,6,9,10 };
	std::cout << std::make_pair(Obj1, Mode::space);
	std::cout << std::make_pair(Obj1, Mode::new_line);

	return 0;
}
