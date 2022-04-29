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
#include "GenericVectorT.hpp"
#include "ForwardCountingIterator.hpp"

#include "Vector1DAs2DT.hpp"

void freeFunction(int arg, std::string str)
{
	std::cout << "Calling free function: \"Hello Signal\" - " << arg << " " << (str) << "\n";
}

constexpr auto lmd = [](int arg, std::string str)
{
	std::cout << "Calling Lambda function: \"Hello Signal\" - " << arg << " " << (str) << "\n";
};

template<typename Type>
class TestSignal final
{
	Type mSignal;

	struct Context {};
public:
	TestSignal() = default;

	void test1()
	{

		mSignal.connect(&freeFunction);
		mSignal.connect(&lmd);

		mSignal.emit(1, "string");
	}

	void test2()
	{
		{
			// @todo : impliment the context object case
			// Context obj;
			// mSignal.connect(&obj, &freeFunction);
			// mSignal.connect(&obj, &lmd);
		}

		mSignal.emit(1, "string");
	}

};


int main()
{
	std::vector<int> vec{ 1, 2, 3, 4, 5, 6 };

	Vector1DAs2D<int> ob{2, 3};
	ob.assign(vec.cbegin(), vec.cend());

	std::cout << ob.width() << " " << ob.height() << "\n";
	
	//ob.print();

	// std::cout << ob[0][1];
	print2(ob);

#if 0 // Test : SignalsT<>
	
	TestSignal<JeJo::Signal<void(int, std::string)>> testSignalObj;
	testSignalObj.test1();
	testSignalObj.test2();

#endif


#if 0 // Test : BinarySearchT<>
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

	// Test - 3: strings
	JeJo::BinarySearch<std::string> Arr2{ "s", "a" };
	Arr2.search("z") ?
		std::cout << "z" << " found at index: " << Arr2.getIndex() << "\n" :
		std::cout << "z" << " Not\n";
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

#if 0 // Test : PairExtT<>
	using PairExtDD = JeJo::PairExt<double, double>;
	std::vector<PairExtDD> vec2(5, JeJo::make_pair(0.1, 1.1));

	for (const auto& pair : vec2)  std::cout << pair;

	const PairExtDD p{ JeJo::make_pair(0.1, 1.1) };
	std::iota(vec2.begin(), vec2.end(), p);
	std::cout << "\n";

	for (const auto& pair : vec2)  std::cout << pair;
#endif

#if 0 // Test : VectorExt<>
	using JeJo::Mode;
	JeJo::VectorExt<int64_t> Obj1{ 1,8,2,3,4,7,5,6,9,10 };
	std::cout << std::make_pair(Obj1, Mode::space);
	std::cout << std::make_pair(Obj1, Mode::new_line);
#endif


#if 0 // Test : GenericVectorT<>	
	GenericVectorT<float> mat1D{1.58f, 2.f, 3.4f};
	std::cout << "Main() - mat1D Result: " << mat1D.updateMinMax() << "\n\n";


	GenericVectorT<GenericVectorT<float>> mat2D{ {1.58f, 2.f, 3.4f}, {-1.58f, 20.f, 3.4f}, {100.f, 200.11f } };
	std::cout << "Main() - mat2D Result: " << mat2D.updateMinMax() << "\n\n";
	mat2D.emplace_back(GenericVectorT<float>{ -1000.f }); // std::vector will also work!
	std::cout << "Main() - mat2D Result: " << mat2D.updateMinMax() << "\n\n";


	GenericVectorT<GenericVectorT<GenericVectorT<int>>> mat3D
	{
		{ 
			{5, 4, 8}, {-8, 9, 8, 8 }
		},
		{ 
			{15, 40, 88}, {-80 }
		} 
	};
	std::cout << "Main() - mat2D Result: " << mat3D.updateMinMax({0, 40}) << "\n\n";
#endif

#if 0 // Test : Forward and reverse iterator by indexes.
	std::vector<int> vec(10);

	std::cout << "size of the vector: " << std::size(vec) << '\n';

	std::cout << "Range [0, ArraySize): "; JeJo::testForward(std::size(vec));
	std::cout << "Range (ArraySize, 0]: "; JeJo::testReverse(std::size(vec));

	JeJo::TArray<float> arr(1);
	std::cout << "Range [0, ArraySize): "; JeJo::testForward(std::size(arr));
	std::cout << "Range (ArraySize, 0]: "; JeJo::testReverse(std::size(arr));
#endif
	return 0;
}

