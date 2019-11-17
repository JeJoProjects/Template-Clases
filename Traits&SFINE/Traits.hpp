#include <type_traits>
#include <iostream>

/* traits for finding the class type and, return type of the member function,
 * from the pointer to the member function of any arbitrary class.
 */

// https://stackoverflow.com/questions/52317134

template<typename Class> struct class_traits final{};

template<typename ReType, typename Class, typename... Args>
struct class_traits<ReType(Class::*)(Args...)> final
{
	using class_type = Class;
	using ret_type = ReType;
};

template<typename MemFunctionPtr> using  class_type = typename class_traits<MemFunctionPtr>::class_type;
template<typename MemFunctionPtr> using  ret_type = typename class_traits<MemFunctionPtr>::ret_type;


struct MyStruct
{
	int foo(int a) /*const noexcept*/ { std::cout << "foo from MyStruct...: " << a << "\n"; return a; }
};