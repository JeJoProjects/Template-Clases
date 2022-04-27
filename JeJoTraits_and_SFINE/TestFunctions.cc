#include "TestFunctions.hpp"


JEJO_BEGIN

// ---------- SFINAE_HPP ---------
void sfinae_test() noexcept
{
	std::cout << Function(nullptr) << " ";
	std::cout << Function(2) << " ";
	std::cout << Function(2.0f) << " ";
	std::cout << Function((2.0)) << "\n";
}


// ---------- TEMPLATE_FUNCTION_HPP ---------
#pragma region template_functions 
MyClass::MyClass(int a, double d, std::string str) noexcept
	: m_data1{ a }
	, m_data2{ d }
	, m_data3{ std::move(str) }
{}

void MyClass::printData1(int a) const noexcept
{
	std::cout << "m_data1: " << m_data1 << " " << a << " ";
}

void MyClass::printData2(int a, double d) const noexcept
{
	std::cout << "m_data2: " << m_data2 << " " << a << " " << d << " ";
}

void MyClass::printData3(const std::string& str) const noexcept
{
	std::cout << "m_data3: " << m_data3 << " " << str << "\n";
}
#pragma endregion


// ---------- TRAITS_HPP ---------
#pragma region traits

int traits::MyStruct::memfunc1(int a)
{
	std::cout << "calling int memfunc1(int a): " << a << "\n"; return a;
}

void traits::MyStruct::memfunc2(double a) const noexcept
{
	std::cout << "calling void memfunc2(double a) const noexcept: " << a << "\n";
}

traits::ClassTraitTest::ClassTraitTest(std::string_view&& str) noexcept
	: mStrMsg{ std::move(str) }
{
	std::cout << "\n\n" << mStrMsg << '\n';
}

void traits::ClassTraitTest::test() const noexcept
{
#if FLAG
	class_type<decltype(&MyStruct::memfunc1)> obj1{};
	[[maybe_unused]] auto ret1 = obj1.memfunc1(1);
	static_assert(std::is_same_v<int, ret_type<decltype(&MyStruct::memfunc1)>>
		, " are not same type!");

	class_type<decltype(&MyStruct::memfunc2)> obj2{};
	obj2.memfunc2(2.22);
	static_assert(std::is_same_v<void, ret_type<decltype(&MyStruct::memfunc2)>>
		, " are not same type!");
#endif
}

	// ------------ HasVector -------------------

traits::IsSpecializationOfTest::IsSpecializationOfTest(std::string_view&& str) noexcept
	: mStrMsg{ std::move(str) }
{
	std::cout << "\n\n" << mStrMsg << "\n\n";
}

void traits::IsSpecializationOfTest::test2() const noexcept
{
#if FLAG
	std::cout << std::boolalpha
		<< "HasVector<A>: " << HasVector<A> << '\n'
		<< "HasVector<B>: " << HasVector<B> << "\n\n\n";

	static_assert(HasVector<A>, " does not return a std::vector!");
	static_assert(HasVector<B>, " does not return a std::vector!");
#endif
}

	// ------------------ has_member -------------

void traits::has_member_function::test()
{
	std::vector<int> V{ 1,9,5,3 };
	A a;
	B b;
	std::cout << "Type == std::vector<int>: "; fast_sort(V);
	std::cout << "Type == struct A "; fast_sort(a);
	std::cout << "Type == struct B with B::sort() : "; fast_sort(b);
}

#pragma endregion 


JEJO_END
