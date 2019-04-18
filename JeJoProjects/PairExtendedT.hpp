/******************************************************************************

******************************************************************************/

#ifndef JEJO_PAIR_EXTENDED_T_HPP
#define JEJO_PAIR_EXTENDED_T_HPP

 // macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END }

// C++ headers
#include <iostream>

JEJO_BEGIN

// forward declaration(s)
template<typename T, typename U> struct PairExt;
template<typename T, typename U>
std::ostream& operator<<(std::ostream &out, const PairExt<T, U>  &p) noexcept;

// PairExt<> definition
template<typename T, typename U>
struct PairExt final
{
	T first{};
	U second{};

	PairExt(const T &fir, const U &sec) noexcept : first{fir}, second{sec} {}

	// Copy : enabled
	PairExt(const PairExt &other) noexcept
		: first{ other.first },
		second{ other.second }
	{}
	PairExt& operator=(const PairExt &other) noexcept
	{
		this->first = other.first;
		this->second = other.second;
		return *this;
	}

	// Move : enabled
	explicit constexpr PairExt(PairExt &&other) noexcept
		: first{std::move(other.first)},
		second{std::move(other.second)}
	{}

	PairExt& operator=(PairExt &&other) noexcept
	{
		this->first = std::move(other.first);
		this->second = std::move(other.second);
		return *this;
	}

	// overloaded operator(s)  of the class
	// @todo: should be enabled only for integral types!
	PairExt& operator++() noexcept
	{
		this->first++; this->second++;
		return *this;
	}

	// post-increment, return unmodified copy by value
	PairExt operator++(int) noexcept
	{
		PairExt copy{ *this };
		++(*this); // or operator++();
		return copy;
	}

	// non-member function(s)
	friend std::ostream& operator<< <>(
		std::ostream & out, const PairExt  &p) noexcept;
};

template<typename T, typename U>
std::ostream& operator<<(std::ostream & out, const PairExt<T, U>  &p) noexcept
{
	return out << p.first << " " << p.second << std::endl;
}

// make_pair() to construct a PairExt<> : similar to std::make_pair
template<typename T, typename U>
PairExt<T, U> make_pair(T first, U second) noexcept
{
	return PairExt<T, U>{first, second};
}

JEJO_END

#endif // JEJO_PAIR_EXTENDED_T_HPP

/*****************************************************************************/
