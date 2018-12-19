/* 
 */

#include <iostream>

template<typename T, typename U>
struct Pair;

template<typename T, typename U>
std::ostream& operator<<(std::ostream & out, const Pair<T, U>  &p);

template<typename T, typename U>
struct Pair : public std::pair<T, U>
{
	using std::pair<T, U>::pair;
	Pair& operator++()
	{
		this->first++; this->second++;
		return *this;
	}
	Pair operator++(int) // post-increment, return unmodified copy by value
	{
		Pair copy(*this);
		++(*this); // or operator++();
		return copy;
	}
	friend std::ostream& operator<< <>(std::ostream & out, const Pair  &p);
};

template<typename T, typename U>
std::ostream& operator<<(std::ostream & out, const Pair<T, U>  &p)
{
	return out << p.first << " " << p.second << std::endl;
}

