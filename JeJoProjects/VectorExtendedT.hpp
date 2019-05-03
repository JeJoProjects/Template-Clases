/******************************************************************************
 * VectorExt is a template class which has been public-ally inherited from
 * std::vector<> class to extend the printing functionality
 *
 * depending upon the enum Mode, operator<< of the class will print the
 * elements stored inside the container.
 *
 * @Authur :  JeJo
 * @Date   :  May - 2018
 * @license: free to use and distribute(no further support as well)
 *****************************************************************************/

#ifndef JEJO_VECTOR_EXTENDED_T_HPP
#define JEJO_VECTOR_EXTENDED_T_HPP

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// C++ headers
#include <iostream>
#include <vector>

JEJO_BEGIN

// enum for specifying the printing style at each elements
enum class Mode { new_line, space };

// forward declaration
template<typename Type> class VectorExt;

// convenience type
template<typename Type>
using ModePair = std::pair<VectorExt<Type>, Mode>;

// forward declaration
template<typename Type>
std::ostream& operator<<(
	std::ostream& out, const ModePair<Type> &pairObjs) noexcept;

// class definition
template<typename Type> class VectorExt final : public std::vector<Type>
{
	using std::vector<Type>::vector;
public:
	// extended functionality: specialization of operator<< for template "Type".
	template<typename U> friend std::ostream& operator<< <>(
		std::ostream& out, const ModePair<U> &pairObjs) noexcept;
};

// definition of non-member function
template<typename Type>
std::ostream& operator<<(std::ostream& out, const ModePair<Type> &pairObjs) noexcept
{
	if (pairObjs.second == Mode::new_line)  // '\n' case
	{
		for (const Type& element : pairObjs.first) out << element << '\n';
		return out;
	}
	for (const Type& element : pairObjs.first) out << element << " ";
	return out << '\n';
}

JEJO_END

#endif // JEJO_VECTOR_EXTENDED_T_HPP

/*****************************************************************************/
