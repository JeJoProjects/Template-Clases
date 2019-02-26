/*
 */

#include <iostream>

template<typename T, typename U> struct PairExt;

template<typename T, typename U>
std::ostream& operator<<(std::ostream & out, const PairExt<T, U>  &p) noexcept;

template<typename T, typename U>
struct PairExt final : public std::pair<T, U>
{
	using std::pair<T, U>::pair;

	PairExt& operator++() noexcept
	{
		this->first++; this->second++;
		return *this;
	}

	PairExt operator++(int) noexcept // post-increment, return unmodified copy by value
	{
		PairExt copy(*this);
		++(*this); // or operator++();
		return copy;
	}

	friend std::ostream& operator<< <>(
		std::ostream & out, const PairExt  &p) noexcept;
};

template<typename T, typename U>
std::ostream& operator<<(std::ostream & out, const PairExt<T, U>  &p) noexcept
{
	return out << p.first << " " << p.second << std::endl;
}

