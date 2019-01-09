// C++ headers
#include <iostream>

// own header
#include "TemplateClass BinarySearch.h"

// test code
int main()
{
	// Test - 1: integers
	BinarySearch<int> Arr0{ 1,  2,  3, 4, 5, 8 };

	for (int i = 1; i <= 10; ++i)
		Arr0.search(i) ?
		std::cout << i << " found at index: " << Arr0.getIndex() << '\n' : std::cout << i << " Not\n";

	std::cout << "\n\n";

	// Test - 2: floats
	BinarySearch<float> Arr1{ 1.04f, 2.04f,  3.04f, 4.04f, 5.04f, 8.04f };

	for (float i = 1; i <= 10.0f; ++i)
		Arr1.search(i + 0.04f) ?
		std::cout << i + 0.04f << " found at index: " << Arr1.getIndex() << '\n' : std::cout << i + 0.04f << " Not\n";

	std::cout << "\n\n";

	// Test - 3: strings
	BinarySearch<std::string> Arr2{"s", "a"};
	Arr2.search("z") ?
		std::cout << "z" << " found at index: " << Arr2.getIndex() << "\n" :
		std::cout << "z" << " Not\n";

	return 0;
}