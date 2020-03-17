#include <utility>  // std::move, std::forward, std::index_sequence
#include <tuple>    // std::tuple, std::make_tuple, std::apply

/******************************************************************************
 * Following helper functions can be used to call more than one member function
 * of a class type to arbitrary number of objects of a class in a loop.
 */

// https://stackoverflow.com/questions/58427341

template < typename T, typename... Ts >
auto getMemberFunction(const std::tuple<T, Ts...>& t)
{
	return std::get<0>(t);
}

template <std::size_t... Ns, typename... Ts >
auto make_args_tuple(std::index_sequence<Ns...>, const std::tuple<Ts...>& t)
{
	return std::make_tuple(std::get<Ns + 1u>(t)...);
}

template <typename... Ts >
auto getArgs(std::tuple<Ts...> const& t)
{
	return ::make_args_tuple(std::make_index_sequence<sizeof...(Ts) - 1u>{}, t);
}

// example code
#include <iostream>
#include <string>
#include <vector>
using namespace std::string_literals;

namespace JeJo
{
	struct MyClass final
	{
	private:
		int m_data1{ 1 };
		double m_data2{ 2.05 };
		std::string m_data3{ "string" };

	public:
		MyClass(int a, double d, std::string str)
			: m_data1{ a }
			, m_data2{ d }
			, m_data3{ std::move(str) }
		{}
		void printData1(int a) const noexcept
		{
			std::cout << "m_data1: " << m_data1 << " " << a << " ";
		}
		void printData2(int a, double d) const noexcept
		{
			std::cout << "m_data2: " << m_data2 << " " << a << " " << d << " ";
		}
		void printData3(const std::string& str) const noexcept
		{
			std::cout << "m_data3: " << m_data3 << " " << str << "\n";
		}
	};

	std::vector<MyClass> objVec{ {1, 1.011, "one"s}, {2, 2.021, "two"s}, {3, 3.031, "three"s} };

	template<typename... Tuples>
	void callMemsForAllObjects(Tuples&&... tuples)
	{
		for (const MyClass& obj : objVec)
		{
			(std::apply(
				::getMemberFunction(std::forward<Tuples>(tuples)),
				::std::tuple_cat(
					std::make_tuple(obj), ::getArgs(std::forward<Tuples>(tuples))
					)
				), ...);
		}
	}
} // namespace JeJo


/**********************************************************************************************************
 *
 */
