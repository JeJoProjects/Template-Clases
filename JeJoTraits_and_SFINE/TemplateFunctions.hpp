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


/************************************************************************************
 *
 */
// stringify ranges(i.e. std::vector like)

#include <string>
#include <type_traits>
#include <string>
#include <concepts>
#include <ranges>
using namespace std::string_literals;

// concept for checking std::to_string-able types
template<typename Type>
concept is_stringable = requires (Type t) { {std::to_string(t) }->std::same_as<std::string>; };

constexpr auto to_string(is_stringable auto&& arg) ->decltype(auto) {
    return std::to_string(std::forward<decltype(arg)>(arg));
}

constexpr void pop_n_backs(std::string& str, std::size_t N) {
    while(N--) str.pop_back();
}

constexpr auto stringify(const std::ranges::range auto& data
    , const std::string& delim = { ", "s })  -> decltype(auto)
{
    // value type of the ranges (i.e. Sequence ranges)
    using ValueType = std::remove_const_t<
        std::remove_reference_t<decltype(*data.cbegin())>>;

    if constexpr (is_stringable<ValueType>) {
        std::string string{};
        for (ValueType element : data) {
            string += to_string(element) +  delim ;
        }
        pop_n_backs(string, std::size(delim));
        return " ["s + string + "] "s;
    }
    else
    {
        std::string string;
        for (const ValueType& innerRange : data)
            string += stringify(innerRange);
        return string;
    }
}

// std::vector<int> a{ 1, 2, 3, 4, 5, 6 };
// std::vector<std::vector<int>> b { { 1, 2 }, { 3, 4 } };

// std::cout << stringify(a, ", "s) << std::endl;
// std::cout << stringify(b) << std::endl;

/************************************************************************************
 *
 */
 // overall size of an multi dim - std::array!!!!
// type traits with recursive template instantiation!
template<typename T> struct arr_sz final {
    inline static constexpr std::size_t size{ sizeof(T) };
};

template<typename T, std::size_t N> struct arr_sz<std::array<T, N>> final {
    inline static constexpr std::size_t size{ N * arr_sz<T>::size };
};

// or using recursive function call
template<typename T> struct is_std_array final : std::false_type {};
template<typename T, std::size_t N> struct is_std_array<std::array<T, N>> final : std::true_type {};
template<typename T> inline constexpr bool is_std_array_t = ::is_std_array<T>::value;

template<typename Type, std::size_t N>
constexpr std::size_t overall_size(const std::array<Type, N>&) noexcept
{
    if constexpr (JeJo::is_std_array_t<Type>) return N * overall_size(Type{});
    return N * sizeof(Type);
}

// Usage
// std::array<std::array<std::array<std::array<double, 2>, 3>, 6>, 100> my_array;
// std::cout << sizeof(my_array) << '\n';
// std::cout << overall_size(my_array) << '\n';
// std::cout << arr_sz<decltype(my_array)>::size << '\n';


JEJO_END // namespace JeJo

#endif // !TEMPLATE_FUNCTION_HPP

