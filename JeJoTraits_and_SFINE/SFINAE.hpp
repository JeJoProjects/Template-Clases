#ifndef SFINAE_HPP
#define SFINAE_HPP
/*******************************************************************************
 * A simple template-function overloads for basic C++ types using the STD traits
 * std::enable_if, std::is_integral and std::is_floating_point.
 * written for C++11 and later!
 */

#include <iostream>
#include <type_traits>

//template alias: convenience types
template<typename Type>
using enable_for_integral
	= typename std::enable_if<std::is_integral<Type>::value, int>::type;

template<typename Type>
using enable_for_floating_points
	= typename std::enable_if<std::is_floating_point<Type>::value, float>::type;

// examples
namespace JeJo
{
	// case 1: No specification of argument-> free to choose
	inline int Function(...)
	{
		return 0;
	}

	// case 2: Only for integers
	template <typename T>
	enable_for_integral<T> Function(const T val)
	{
		return 1;
	}

	// case 3: First priority than the templates version below
	inline float Function(const float val)
	{
		return val; // or as follows
	}

	// case 4: Other floating point family cases(Ex: 4th long double or double cases)
	template <typename T>
	enable_for_floating_points<T> Function(const T val)
	{
		return 2;
	}
} // namespace JeJo

#endif // SFINAE_HPP


