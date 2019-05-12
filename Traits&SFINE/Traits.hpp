/* 
 */

// https://stackoverflow.com/questions/52317134

template<typename Class> struct get_class {};

template<typename ReType, typename Class, typename... Args>
struct get_class<ReType(Class::*)(Args...)>
{
	using type = Class;
};

template<typename MemFunc>
using get_class_t = typename get_class<MemFunc>::type;


struct MyStruct
{
	void foo() /*const noexcept*/ { std::cout << "foo from MyStruct...\n"; }
};