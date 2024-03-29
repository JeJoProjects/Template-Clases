#ifndef TEST_FUNCTIONS_HPP
#define TEST_FUNCTIONS_HPP

#include <vector>
#include <string>
using namespace std::string_literals;

#include "SFINAE.hpp"
#include "TemplateFunctions.hpp"
// #include "Traits.hpp"



// ---------- SFINAE_HPP ---------
inline void sfinae_test() noexcept
{
	std::cout << Function(nullptr) << " ";
	std::cout << Function(2) << " ";
	std::cout << Function(2.0f) << " ";
	std::cout << Function((2.0)) << "\n";
}


// ---------- TEMPLATE_FUNCTION_HPP ---------
#pragma region template_functions

struct MyClass final
{
private:
	int m_data1{ 1 };
	double m_data2{ 2.05 };
	std::string m_data3{ "string"s };

public:
	MyClass(int a, double d, std::string str) noexcept : m_data1{ a }
		, m_data2{ d }
		, m_data3{ std::move(str) }
	{}

	void printData1(int ) const noexcept {
		std::cout << "call... MyClass::printData1(int a) const noexcept\n";
	}

	void printData2(int , double ) const noexcept {
		std::cout << "call... MyClass::printData2(int a, double d) const noexcept\n";
	}

	void printData3(const std::string& ) const noexcept {
		std::cout << "call... MyClass::printData3(const std::string& str) const noexcept;\n\n";
	}
};


template<typename... Tuples>
constexpr inline void callMemsForAllObjects(Tuples&&... tuples) noexcept
{
	static std::vector<MyClass> objVec{ 
		{1, 1.011, "one"s}, {2, 2.021, "two"s}, {3, 3.031, "three"s} 
	};

	for (const MyClass& obj : objVec)
	{
		(std::apply(
			getMemberFunction(std::forward<Tuples>(tuples)),
			std::tuple_cat(
				std::make_tuple(obj), getArgs(std::forward<Tuples>(tuples))
			)
		), ...);
	}
}

inline void callMemsForAllObjects_test()
{
	callMemsForAllObjects(
		std::make_tuple(&MyClass::printData1, 200),
		std::make_tuple(&MyClass::printData2, 201, 50.55),
		std::make_tuple(&MyClass::printData3, "test string"s)
	);
}


inline void print_args_test()
{
	print_args_forward(1, 2.4f, 'd', "string"s);
	print_args_backwards(1, 2.4f, 'd', "string"s);
}

inline void stringify_test()
{
	std::vector<int> a{ 1, 2, 3, 4, 5, 6 };
	std::vector<std::vector<int>> b{ { 1, 2 }, { 3, 4 } };

	std::cout << "\n\n" << stringify(a, ", "s) << '\n';
	std::cout << stringify(b) << "\n\n";
}


#pragma endregion

#if 0
// ---------- TRAITS_HPP ---------
#pragma region traits 

namespace traits
{
	struct MyStruct final
	{
		int memfunc1(int a);
		void memfunc2(double a) const noexcept;

	};

	class ClassTraitTest final
	{
		std::string_view mStrMsg{};

	public:
		explicit ClassTraitTest(std::string_view&& str) noexcept;
		void test() const noexcept;
	};


	// -------------- HasVector -----------------
	struct A {
		std::vector<int> vec() { return {}; }
	};

	struct B {
		std::vector<double> vec() { return {}; }
	};

	class IsSpecializationOfTest final
	{
		std::string_view mStrMsg{};

	public:
		explicit IsSpecializationOfTest(std::string_view&& str) noexcept;
		template<int I = 1> void test1() const noexcept
		{
			std::vector<std::vector<int>> vec;
#if FLAG
			std::cout << std::boolalpha
				<< "is_specialization_of<decltype(vec), std::vector>: "
				<< is_specialization_of<decltype(vec), std::vector> << '\n';
			static_assert(is_specialization_of<decltype(vec), std::vector>, " is not a std::vector!");

#elif !FLAG
			std::cout << std::boolalpha
				<< "is_specialization_of<decltype(vec), std::vector>::value: "
				<< is_specialization_of<decltype(vec), std::vector>::value << '\n';
			static_assert(is_specialization_of<decltype(vec), std::vector>::value, " is not a std::vector!");
#endif
		}
		void test2() const noexcept;
	};

	// ------------------ has_member -------------

	class has_member_function final
	{
	private:
		class A {};
	class B { public: void sort() {} };

	template<typename T>
	void fast_sort(T& x) noexcept
	{
		if constexpr (has_sort_v<T>) {
			std::cout << "has_sort\n";
			x.sort();
		}
		else if constexpr (has_range_v<T>) {
			std::cout << "has_range\n";
			std::sort(std::begin(x), std::end(x));
		}
		else {
			std::cout << "No member function found\n";
		}
	}
	public:

		void test();
	};
}
#pragma endregion 

#endif


#endif // !TEST_FUNCTIONS_HPP

