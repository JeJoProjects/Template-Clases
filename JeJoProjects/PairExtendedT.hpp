/******************************************************************************
 * Implementation of PairExt<>, which provides a template class for a pair type
 * which accepts two arbitrary types as template parameters.
 *
 * @Authur :  JeJo
 * @Date   :  May - 2018
 * @license: free to use and distribute(no further support as well)
 *
 * The non-member functions are restricted to be not to instantiate for the
 * types other than the allowed_arithmetic types.
 * Traits 'is_allowed_arithmetic<>' will ensure this to happens.
 *
 * Do not wonder:
 *   - Why I haven't used static_assert() over the SFINE. Just for func! ;)
******************************************************************************/

#ifndef JEJO_PAIR_EXTENDED_T_HPP
#define JEJO_PAIR_EXTENDED_T_HPP

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END }

// C++ headers
#include <iostream>
#include <type_traits>

JEJO_BEGIN

// traits for possible(arithmetic) template types(s)
template<typename Type>
using is_allowed_arithmetic = std::conjunction<
   std::is_arithmetic<Type>,
   std::negation<std::is_same<Type, bool>>,
   std::negation<std::is_same<Type, char>>,
   std::negation<std::is_same<Type, char16_t>>,
   std::negation<std::is_same<Type, char32_t>>,
   std::negation<std::is_same<Type, wchar_t>> >;
 /*std::negation<std::is_same<char8_t, Type>> */ // valid since C++20

template<typename T, typename U, typename ReType = void>
using is_allowed_arithmetic_t = std::enable_if_t<
	    std::conjunction_v<
            JeJo::is_allowed_arithmetic<T>,
	        JeJo::is_allowed_arithmetic<U>
		>, ReType
   >;

/* forward declaration(s) and conditional instantiation of the template class,
 * depending on the template argument.
 * @todo: should be enabled only for arithmetic types!
 */
template<typename T, typename U, typename Enable = void> struct PairExt;

template<typename T, typename U>
auto operator<<(std::ostream &out, const PairExt<T, U>  &p) noexcept ->
                            JeJo::is_allowed_arithmetic_t<T, U, std::ostream&>;

// PairExt<> definition
template<typename T, typename U> struct PairExt<
	T, U, JeJo::is_allowed_arithmetic_t<T, U, void>
>final
{
	// public members
	T first{};
	U second{};

	// constructor(s)
	constexpr explicit PairExt(const T &fir, const U &sec) noexcept
		: first{ fir }
		, second{ sec }
	{}
	// copy : enabled
	constexpr PairExt(const PairExt& other) noexcept = default;
	constexpr PairExt& operator=(const PairExt& other) noexcept = default;
	// move : enabled
	constexpr PairExt(PairExt &&other) noexcept = default;
	constexpr PairExt& operator=(PairExt&& other) noexcept = default;

	// overloaded operator(s)
	constexpr PairExt& operator++() noexcept
	{
		this->first++;
		this->second++;
		return *this;
	}
	// post-increment, return unmodified copy by value
	constexpr PairExt operator++(int) noexcept
	{
		const PairExt copy{ *this };
		++(*this); // or operator++();
		return copy;
	}

	// non-member function(s): enabled only for the allowed_arithmetic types
	friend auto operator<< <>(
		std::ostream & out, const PairExt  &p) noexcept ->
		                    JeJo::is_allowed_arithmetic_t<T, U, std::ostream&>;
};

template<typename T, typename U>
auto operator<<(std::ostream & out, const PairExt<T, U>  &p) noexcept ->
                           JeJo::is_allowed_arithmetic_t<T, U, std::ostream&>
{
	return out << p.first << " " << p.second << std::endl;
}

/* make_pair() to construct a PairExt<> : similar to std::make_pair
 * This has been enabled only for the allowed_arithmetic types.
 */
template<typename T, typename U>
auto make_pair(T first, U second) noexcept ->
                            JeJo::is_allowed_arithmetic_t<T, U, PairExt<T, U>>
{
	return PairExt<T, U>{first, second};
}

JEJO_END

#endif // JEJO_PAIR_EXTENDED_T_HPP

/*****************************************************************************/
