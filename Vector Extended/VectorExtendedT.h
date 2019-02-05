/* VectorExt is a template class which has been publically inherited from
 * std::vector<> class to extend the printing functionality
 *
 * depending upon the enum Mode, operator<< of the class will print the
 * elements stored inside the container.
 */

#include <iostream>

// enum for for specifying the printing style at each elements
enum class Mode { new_line, space };
// forward declaration
template<typename Type> class VectorExt;

// convenience type
template<typename Type>
using Pair = std::pair<VectorExt<Type>, Mode>;

// forward declaration
template<typename Type>
std::ostream& operator<<(std::ostream& out, const Pair<Type> &pairObjs) noexcept;

// class definition
template<typename Type> class VectorExt final : public std::vector<Type>
{
	using std::vector<Type>::vector;
public:
	// extended functionality: specilization of operator<< for template "Type".
	template<typename Type>
	inline friend std::ostream& operator<<<>(std::ostream& out, const Pair<Type> &pairObjs) noexcept;
};

// definition of non-member function
template<typename Type>
std::ostream& operator<<(std::ostream& out, const Pair<Type> &pairObjs) noexcept
{
	if (pairObjs.second == Mode::new_line)  // '\n' case
	{
		for (const Type& element : pairObjs.first)
			out << element << '\n';
	}
	else if (pairObjs.second == Mode::space)   // '\t' case
	{
		for (const Type& element : pairObjs.first)
			out << element << " ";
		out << '\n';
	}
	return out;
}