
#include "TestFunctions.hpp"


#if 0

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


#endif