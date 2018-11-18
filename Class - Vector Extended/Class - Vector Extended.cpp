di/* VectorExt is a template class which has been publically inherited from
 * std::vector<> class to extend the printing functionality
 *
 * depending upon the enum Mode, operator<< of the class will print the
 * elements stored inside the container.
 */

#include <iostream>
#include <vector>
#include <cstdint>

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
	// extended functionality: specilization of operator<< for Type.
	template<typename Type>
	friend std::ostream& operator<<<>(std::ostream& out, const Pair<Type> &pairObjs) noexcept;
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

// test code
int main()
{
	VectorExt<int> obj(10);
	std::cout << std::make_pair(obj, Mode::new_line);
	std::cout << std::make_pair(obj, Mode::space);

	VectorExt<int64_t> Obj1 = { 1,8,2,3,4,7,5,6,9,10 };
	std::cout << std::make_pair(Obj1, Mode::space);
	std::cout << std::make_pair(Obj1, Mode::new_line);

	return 0;
}
