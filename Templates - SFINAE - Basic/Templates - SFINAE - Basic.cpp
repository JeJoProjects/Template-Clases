#include <iostream>
#include <type_traits>

// case 1: No specification of argument-> free to choose
int Function(...) { return 0; }

// case 2: Only for integers
template <typename T>
typename std::enable_if<std::is_integral<T>::value, int>::type
	Function(T val) { return 1; }

// case 3: First prority than the templated version below
int Function(float val) { return 2; } // or as foloows

// case 4: Other floating point family cases(Ex: 4th long double or double cases)
template <typename T>
std::enable_if_t<std::is_floating_point<T>::value, int> 
	Function(T val) { return 2; }

int main()
{
	std::cout << Function(nullptr)	<< " ";
	std::cout << Function(2)		<< " ";
	std::cout << Function(2.0f)		<< " ";
	std::cout << Function((2.0))	<< "\n";

	std::cin.get();
}