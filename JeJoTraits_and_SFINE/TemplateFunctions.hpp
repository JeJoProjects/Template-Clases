#ifndef TEMPLATE_FUNCTION_HPP
#define TEMPLATE_FUNCTION_HPP

#include <iostream>
#include <string>
#include <utility>      // std::move, std::forward, std::index_sequence
#include <tuple>        // std::tuple, std::make_tuple, std::apply
#include <version>      // __has_include, __cpp_lib_type_identity
#include <type_traits>  // std::conditional_t
using namespace std::string_literals;


// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

JEJO_BEGIN
/******************************************************************************
 * Following helper functions can be used to call more than one member function
 * of a class type to arbitrary number of objects of a class in a loop.
 */

// Inspired from: https://stackoverflow.com/questions/58427341

template <typename T, typename... Ts>
auto getMemberFunction(const std::tuple<T, Ts...>& t) noexcept
{
	return std::get<0>(t);
}

template <std::size_t... Ns, typename... Ts >
auto make_args_tuple(std::index_sequence<Ns...>, const std::tuple<Ts...>& t) noexcept
{
	return std::make_tuple(std::get<Ns + 1u>(t)...);
}

template <typename... Ts >
auto getArgs(std::tuple<Ts...> const& t) noexcept
{
	return make_args_tuple(std::make_index_sequence<sizeof...(Ts) - 1u>{}, t);
}

/************************************************************************************
 *
 */
// iterate forward and backwards!!!!

#if __has_include(<version>) and __cpp_lib_type_identity

template<typename Type>
using conditional_const_ref = std::conditional_t<std::is_fundamental_v<Type>
    , Type
    , std::add_const_t<std::add_lvalue_reference_t<Type>>
>;

template<typename... Args>
void print_args(Args&&... args) noexcept
{
    constexpr auto print_helper = []<typename Type>
        (conditional_const_ref<Type> arg) constexpr noexcept
    {
        std::cout << arg << '\n';
        return std::type_identity<void>{};
    };

    static constexpr auto print = [print_helper]<typename Type>
        (Type&& arg) constexpr noexcept
    {
        return print_helper.template operator()<Type> (std::forward<Type>(arg));
    };

    std::cout << "\n\n*** Backwards ***\n";
    (print(args) = ...);

    std::cout << "\n*** Forwards ***\n";
    (print(args), ...);
}

#endif // __has_include(<version>) and __cpp_lib_type_identity

JEJO_END // namespace JeJo

#endif // !TEMPLATE_FUNCTION_HPP

