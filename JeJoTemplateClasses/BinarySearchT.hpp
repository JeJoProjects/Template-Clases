/******************************************************************************
 * Implementation of BinarySearch<>, which provides a template container class
 * to binary search the element in the internally stored array.
 *
 * @Authur :  JeJo
 * @Date   :  June - 2018
 * @license: free to use and distribute(no further support as well)
 *
 * @todo: explanation and limitations of the class
 *****************************************************************************/

#ifndef JEJO_BINARY_SEARCH_T_HPP
#define JEJO_BINARY_SEARCH_T_HPP

 // macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// C++ headers
#include <iostream>
#include <vector>            // std::vector<>
#include <limits>            // std::numeric_limits<>
#include <algorithm>         // std::is_sorted
#include <exception>         // std::runtime_error
#include <initializer_list>  // std::initializer_list<>
#include <type_traits>       // std::is_arithmetic_v<>
#include <cstddef>           // std::size_t

// own JeJo-lib headers
#include "JeJoAlgorithumsT.hpp"

JEJO_BEGIN

// variable template for convenience use of std::numeric_limits<>
template<typename Type>
inline constexpr Type typeMinimum = std::numeric_limits<Type>::min();

// BinarySearch<> definition
template<typename Type> class BinarySearch final
{
private:
	// internal storage of array
	std::vector<Type> _vec;
	// index of the searching element (all other non-integer cases)
	std::size_t _index{ 0 };

private:
	// re-setter to set the default(min) value
	void make_default_index() noexcept
	{
		if constexpr (std::is_arithmetic_v<Type>)
		{
			this->_index = static_cast<std::size_t>(typeMinimum<Type>);
			return;
		}
	}

	// helper function to search specific element in the array
	// and returns: true... if found!
	// Indication to that: save the position of the element in the array
	bool search(std::size_t Start, std::size_t End, const Type& val)
	{
		if (End == 0) return false;
		try
		{
			// if not sorted, do not try to find the value
			if (!std::is_sorted(this->_vec.cbegin(), this->_vec.cend()))
			{
				throw std::runtime_error("Array is not sorted!\n");
			}

			if (Start <= End)
			{
				const std::size_t mid_index = Start + ((End - Start) / 2);
				// Find the middle element -> Check ->  split
				if (this->_vec[mid_index] == val)
				{
					this->_index = mid_index;
					return true;
				}
				else if (this->_vec[mid_index] < val)
				{
					return this->search(mid_index + 1, End, val);
				}
				else
				{
					return this->search(Start, mid_index - 1, val);
				}
			}
		}
		catch (const std::runtime_error& e) { std::cout << e.what() << '\n'; 	}
		this->make_default_index();
		return false;
	}

public:
	// initializer_list constructor
	explicit constexpr BinarySearch(const std::initializer_list<Type> a)
		: /*(!JeJo::has_duplicates(a.begin(), a.end())
		? static_assert(false, " the array has duplicates -> No BS possible!\n")
		: */_vec{ a }//)
	{
		std::cout <<
			std::boolalpha << has_duplicates(_vec.cbegin(), _vec.cend());
		this->make_default_index();
	}

	// getter for index
	std::size_t getIndex() const  noexcept { return this->_index; }

	// member function to search specific elements in the array
	bool search(const Type& val)
	{
		return this->search(
			0,
			this->_vec.empty() ? 0 : this->_vec.size() - 1,
			val);
	}
};

JEJO_END

#endif // JEJO_BINARY_SEARCH_T_HPP

/*****************************************************************************/
