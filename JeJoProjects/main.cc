// C++ headers
#include <vector>
#include <numeric>
#include <iomanip>

// Library headers
#include "BinarySearchT.hpp"
#include "FractionT.hpp"
#include "PairExtendedT.hpp"
// #include "ShapeT.hpp" //@todo: need implementation
#include "SignalsT.hpp"
#include "VectorExtendedT.hpp"


int main()
{
#if 1 // Test : BinarySearchT<>
	// Test - 1: integers
	JeJo::BinarySearch<int> Arr0{ 1,  2,  3, 4, 5, 8 };

	for (int i = 1; i <= 10; ++i)
		Arr0.search(i) ?
		std::cout << i << " found at index: " << Arr0.getIndex() << '\n' : std::cout << i << " Not\n";

	std::cout << "\n\n";

	// Test - 2: floats
	JeJo::BinarySearch<float> Arr1{ 1.04f, 2.04f,  3.04f, 4.04f, 5.04f, 8.04f };

	for (float i = 1; i <= 10.0f; ++i)
		Arr1.search(i + 0.04f) ?
		std::cout << i + 0.04f << " found at index: " << Arr1.getIndex() << '\n' : std::cout << i + 0.04f << " Not\n";

	std::cout << "\n\n";
#if 0
	// Test - 3: strings
	JeJo::BinarySearch<std::string> Arr2{ "s", "a" };
	Arr2.search("z") ?
		std::cout << "z" << " found at index: " << Arr2.getIndex() << "\n" :
		std::cout << "z" << " Not\n";
#endif
#endif

#if 0 // Test : FractionT<>
	for (int i = -10; i <= 15; ++i)
	{
		JeJo::Fraction<int> obj{ i, 10 };
		std::cout << obj << std::setw(8) << obj.getReal() << std::endl;
	}

	// @todo: implementation for float and doubles
	// JeJo::Fraction<float> obj1{3.5f, 1.0f};
	// std::cout << obj1 << "   " << obj1.getReal() << std::endl;
#endif

#if 0 // Test : VectorExt<>
	JeJo::VectorExt<int> obj(10, 1);
	std::cout << std::make_pair(obj, JeJo::Mode::new_line);
	std::cout << std::make_pair(obj, JeJo::Mode::space);

	JeJo::VectorExt<int64_t> Obj1 = { 1,8,2,3,4,7,5,6,9,10 };
	std::cout << std::make_pair(Obj1, JeJo::Mode::space);
	std::cout << std::make_pair(Obj1, JeJo::Mode::new_line);
#endif

#if 0 // Test : SignalsT<>
	const auto lmd = []() { std::cout << "Hello Signal\n"; };
	Signal<void()> sg;
	sg.connect(&lmd);
	sg.emit();
#endif

#if 0 // Test : PairExtT<>
	using PairExtDD = JeJo::PairExt<double, double>;
	std::vector<PairExtDD> vec2(5, JeJo::make_pair(0.1, 1.1));

	for (const auto& pair : vec2)  std::cout << pair;

	const PairExtDD p{ JeJo::make_pair(0.1, 1.1) };
	std::iota(vec2.begin(), vec2.end(), p); std::cout << "\n";

	for (const auto& pair : vec2)  std::cout << pair;
#endif
	return 0;
}

