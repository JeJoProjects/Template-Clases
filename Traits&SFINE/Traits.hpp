#include <type_traits>
#include <iostream>
#include <vector>

/****************************************************************************
 * traits for finding the class type and, return type of the member function,
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

// example
namespace JeJo
{
   struct MyStruct
   {
      int foo(int a) /*const noexcept*/
      {
         std::cout << "foo from MyStruct...: " << a << "\n"; return a;
      }
   };
} // namespace JeJo


/****************************************************************************
 *
 */

// traits for checking the Type is `std::vector`
template<typename> struct is_std_vector final : std::false_type {};
template<typename T, typename... Args>
struct is_std_vector<std::vector<T, Args...>> final : std::true_type {};

// traits for checking the Type is `std::vector<any_plotting point types>`
template<typename> struct is_simple_vector final : std::false_type {};
template<typename T, typename... Args>
struct is_simple_vector<std::vector<T, Args...>> final
{
	static constexpr bool value
      = is_std_vector<T>::value == false && std::is_floating_point_v<T> == true;
};

/****************************************************************************
 *
 */
