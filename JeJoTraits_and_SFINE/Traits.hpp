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

// example and test
struct MyStruct final
{
    int memfunc1(int a)
    {
        std::cout << "calling int memfunc1(int a): " << a << "\n"; return a;
    }

    void memfunc2(double a) const noexcept
    {
        std::cout << "calling void memfunc2(double a) const noexcept: " << a << "\n";
    }
};

class ClassTraitTest final
{
    std::string_view mStrMsg{};

public:
    explicit ClassTraitTest(std::string_view&& str) noexcept
        : mStrMsg{ std::move(str) }
    {
        std::cout << "\n\n" << mStrMsg << '\n';
    }

    void test() const noexcept
    {       
#if FLAG
        class_type<decltype(&MyStruct::memfunc1)> obj1{};
        [[maybe_unused]] auto ret1 = obj1.memfunc1(1);
        static_assert(std::is_same_v<int, ret_type<decltype(&MyStruct::memfunc1)>>
            , " are not same type!");

        class_type<decltype(&MyStruct::memfunc2)> obj2{};
        obj2.memfunc2(2.22);
        static_assert(std::is_same_v<void, ret_type<decltype(&MyStruct::memfunc2)>>
            , " are not same type!");
#endif
    }
};

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

// example and test
struct A
{
    std::vector<int> vec() { return {}; }
};

struct B
{
    std::vector<double> vec() { return {}; }
};

class IsSpecializationOfTest final
{
    std::string_view mStrMsg{};

public:
    explicit IsSpecializationOfTest(std::string_view&& str) noexcept
        : mStrMsg{ std::move(str) }
    {
        std::cout << "\n\n" << mStrMsg << '\n';
    }
    template<int I = 1>
    void test1() const noexcept
    {
        std::vector<std::vector<int>> vec;
#if FLAG
        std::cout << std::boolalpha
            << "is_specialization_of<decltype(vec), std::vector>: "
            << is_specialization_of<decltype(vec), std::vector> << '\n';
        static_assert(is_specialization_of<decltype(vec), std::vector>, " is not a std::vector!");

#elif !FLAG
        std::cout << std::boolalpha
            << "is_specialization_of<decltype(vec), std::vector>::value: "
            << is_specialization_of<decltype(vec), std::vector>::value << '\n';
        static_assert(is_specialization_of<decltype(vec), std::vector>::value, " is not a std::vector!");
#endif
    }

    void test2() const noexcept
    {
#if FLAG
        std::cout << std::boolalpha
            << "HasVector<A>: " << HasVector<A> << '\n'
            << "HasVector<B>: " << HasVector<B> << '\n';

        static_assert(HasVector<A>, " does not return a std::vector!");
        static_assert(HasVector<B>, " does not return a std::vector!");
#endif
    }
};
