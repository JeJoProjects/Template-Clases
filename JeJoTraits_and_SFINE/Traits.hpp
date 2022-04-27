/******************************************************************************
 * Following are a set of traits which are useful in template meta programming.
 *
 * @Authur : JeJo
 * @Date   : Jan - 2018
 * @license: MIT
 *****************************************************************************/

#ifndef JEJO_TRAITS_HPP
#define JEJO_TRAITS_HPP

 // C++ headers
#include <iostream>
#include <vector>
#include <string_view>
#include <type_traits>
#include <concepts>
#include <algorithm>


// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// macro for macro enable!!
#define FLAG 1

JEJO_BEGIN

/****************************************************************************
 * Template Trait for finding the class type and, return type from the pointer to
 * member function, of any arbitrary class.
 * 
 * Inspired from: https://stackoverflow.com/questions/68673845#68673845
 */
#if !FLAG

template<typename Class> struct class_traits final{};
template<typename ReType, typename Class, typename... Args>
struct class_traits<ReType(Class::*)(Args...)> final
{
	using class_type = Class;
	using ret_type = ReType;
};

#elif FLAG
/* Now for the the non - const, non - volatile, and non - noexcept, etc versions
 * of the member function pointers.
 */

template<typename Class> struct class_traits final {};
#define CREATE_CLASS_TRAITS(...) \
template<typename ReType, typename Class, typename... Args> \
struct class_traits<ReType(Class::*)(Args...)__VA_ARGS__> final  \
{ \
    using class_type = Class; \
    using ret_type = ReType; \
}

CREATE_CLASS_TRAITS();
CREATE_CLASS_TRAITS(const);
CREATE_CLASS_TRAITS(volatile);
CREATE_CLASS_TRAITS(const volatile);
CREATE_CLASS_TRAITS(&);
CREATE_CLASS_TRAITS(const&);
CREATE_CLASS_TRAITS(volatile&);
CREATE_CLASS_TRAITS(const volatile&);
CREATE_CLASS_TRAITS(&&);
CREATE_CLASS_TRAITS(const&&);
CREATE_CLASS_TRAITS(volatile&&);
CREATE_CLASS_TRAITS(const volatile&&);
CREATE_CLASS_TRAITS(noexcept);
CREATE_CLASS_TRAITS(const noexcept);
CREATE_CLASS_TRAITS(volatile noexcept);
CREATE_CLASS_TRAITS(const volatile noexcept);
CREATE_CLASS_TRAITS(&noexcept);
CREATE_CLASS_TRAITS(const& noexcept);
CREATE_CLASS_TRAITS(volatile& noexcept);
CREATE_CLASS_TRAITS(const volatile& noexcept);
CREATE_CLASS_TRAITS(&& noexcept);
CREATE_CLASS_TRAITS(const&& noexcept);
CREATE_CLASS_TRAITS(volatile&& noexcept);
CREATE_CLASS_TRAITS(const volatile&& noexcept);
#undef CREATE_CLASS_TRAITS

// Helper types
template<typename MemFunctionPtr>
using  class_type = typename class_traits<MemFunctionPtr>::class_type;

template<typename MemFunctionPtr>
using  ret_type = typename class_traits<MemFunctionPtr>::ret_type;
#endif


/****************************************************************************
 * Template Traits for checking the passed template container class is
 * a specialization for the standard container `std::vector`.
 */

template<typename> struct is_std_vector final : std::false_type {};
template<typename T, typename... Args>
struct is_std_vector<std::vector<T, Args...>> final : std::true_type {};

/****************************************************************************
 * Template Trait for checking the passed template container class is
 * a simple std::vector, which has  `std::vector<any_plotting point types>`.
 * This must be combined with the above is_std_vector<> trait.
 */
template<typename> struct is_simple_vector final : std::false_type {};
template<typename T, typename... Args>
struct is_simple_vector<std::vector<T, Args...>> final
{
	static constexpr bool value
      = is_std_vector<T>::value == false && std::is_floating_point_v<T> == true;
};

/****************************************************************************
 * Template Trait for checking the passed container(i.e first template parameter)
 * is a specialization of the other (i.e second template parameter).
 * 
 * Inspired from: https://stackoverflow.com/questions/68435612#68435612
 */

#if !FLAG // C++11 version
template<typename Type, template<typename...> class Args>
struct is_specialization_of final : std::false_type {};

template<template<typename...> class Container, typename... Args>
struct is_specialization_of<Container<Args...>, Container> : std::true_type {};

// Usage:
// is_specialization_of<ContainerType, std::vector>::value
// is_specialization_of<ContainerType, std::list>::value

#elif FLAG // c++20 version:
/* Using variable template(since C++14) & concepts(C++20)!
 * Inspired from: https://stackoverflow.com/questions/68522454#68522454
 */ 

template <typename Type, template <typename...> class Container>
inline constexpr bool is_specialization_of = false;

template <template <typename...> class Container, typename... Args>
inline constexpr bool is_specialization_of<Container<Args...>, Container> = true;

template <typename Type, template <typename...> class Container>
concept Specializes = is_specialization_of<Type, Container>;

template<typename Type>
concept HasVector = requires (Type t)
{
    { t.vec() } -> Specializes<std::vector>;
};
#endif

/****************************************************************************
 * Template Trait for finding the type of the last argument in the passed
 * a variadic template arguments. 
 * 
 *
 * Inspired from: https://stackoverflow.com/questions/68808318#68808318
 */

// Note: Special case for empty pack (as last_t<> is ill-formed)
template <typename... Args>
using last_variadic_t = typename decltype((std::type_identity<Args>{}, ...))::type;


// or

template<typename... Args> struct last_variadic_type {};
template<typename Type1, typename Type2, typename...Args>
struct last_variadic_type<Type1, Type2, Args...> : public last_variadic_type<Type2, Args...>  {};

template<typename T> struct last_variadic_type<T> {
    using type = T;
};

template<typename...Args>
using last_type_t = typename last_variadic_type<Args...>::type;


/****************************************************************************
 *
 *
 *
 *
 * Inspired from: 
 */
template<class T>
struct has_member final
{
    template<class> static auto SortTest(...)->std::false_type;

    template<class TT>
    static auto SortTest(int) ->
        decltype(std::declval<TT&>().sort(), std::true_type());

    template<class> static auto RangeTest(...)->std::false_type;

    template<class TT>
    static auto RangeTest(int) ->
        decltype(std::begin(std::declval<TT&>()),
            std::end(std::declval<TT&>()),
            std::true_type());

    static constexpr bool sort_value = decltype(SortTest<T>(0))::value;

    static constexpr bool range_value = decltype(RangeTest<T>(0))::value;
};
template<class T> inline static constexpr bool has_sort_v = has_member<T>::sort_value;
template<class T> inline static constexpr bool has_range_v = has_member<T>::range_value;


/*****************************************************************************/

JEJO_END

#endif // JEJO_TRAITS_HPP

