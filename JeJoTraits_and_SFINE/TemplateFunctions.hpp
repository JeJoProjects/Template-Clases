#ifndef TEMPLATE_FUNCTIONS_HPP
#define TEMPLATE_FUNCTIONS_HPP

#include <iostream>
#include <string>
#include <utility>      // std::move, std::forward, std::index_sequence
#include <tuple>        // std::tuple, std::make_tuple, std::apply
#include <version>      // __has_include, __cpp_lib_type_identity
#include <type_traits>  // std::conditional_t
using namespace std::string_literals;


/******************************************************************************
 * Following helper functions can be used to call more than one member function
 * of a class type to arbitrary number of objects of a class in a loop.
 */
// Inspired from: https://stackoverflow.com/questions/58427341

template <typename T, typename... Ts>
constexpr auto getMemberFunction(const std::tuple<T, Ts...>& t)
{
	return std::get<0>(t);
}

template <std::size_t... Ns, typename... Ts >
constexpr auto make_args_tuple(std::index_sequence<Ns...>, std::tuple<Ts...>&& t)
{
	return std::make_tuple(
        std::get<Ns + 1u>(
            std::forward<std::tuple<Ts...>>(t))...
    );
}

template <typename... Ts >
constexpr auto getArgs(std::tuple<Ts...>&& t)
{
	return make_args_tuple(
        std::make_index_sequence<sizeof...(Ts) - 1u>{}
      , std::forward<std::tuple<Ts...>>(t));
}

/************************************************************************************
 *
 */
// iterate forward and backwards!!!!

#if __has_include(<version>) and __cpp_lib_type_identity

template<typename Type>
// using conditional_const_ref = std::conditional_t<std::is_fundamental_v<Type>
//     , Type
//     , std::add_const_t<std::add_lvalue_reference_t<Type>>
// >;
template<typename Type>
using conditional_const_ref = std::conditional_t<std::is_fundamental_v<Type>
    , Type
    , const Type&
>;


template<typename... Args>
void print_args_backwards(Args&&... args) noexcept
{
    constexpr auto print_helper = []<typename Type>
        (const auto & arg) constexpr noexcept
    {
        std::cout << arg << '\n';
        return std::type_identity<void>{};
    };

    static constexpr auto print = [print_helper]<typename Type>
        (Type&& arg) constexpr noexcept
    {
        return print_helper.template operator() < Type > (std::forward<Type>(arg));
    };

    std::cout << "------ Backwards ------\n";
    (print(args) = ...);
}


template<typename... Args>
void print_args_forward(Args&&... args) noexcept
{
    constexpr auto print_helper = []<typename Type>
        (const auto & arg) constexpr noexcept
    {
        std::cout << arg << '\n';
        return std::type_identity<void>{};
    };

    static constexpr auto print = [print_helper]<typename Type>
        (Type && arg) constexpr noexcept
    {
        return print_helper.template operator() < Type > (std::forward<Type>(arg));
    };

    std::cout << "\n------ Forwards ------\n";
    (print(args), ...);
}


#endif // __has_include(<version>) and __cpp_lib_type_identity

/************************************************************************************
 *
 */
// stringify ranges(i.e. std::vector like)
// Inspired from: https://stackoverflow.com/questions/74169313/4169686#74169686

#include <string>
#include <type_traits>
#include <string>
#include <concepts>
#include <ranges>

// concept for checking std::to_string-able types
template<typename Type>
concept is_stringable = requires (Type t) {
     {std::to_string(t) }->std::same_as<std::string>;
    // or
    // requires (requires { { std::to_string(t) }->std::same_as<std::string>; });
};


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




#if 0

/************************************************************************************
 *
 */
// to find the type
#include <string_view>

template <typename T>
constexpr auto type_name() {
    std::string_view name, prefix, suffix;
#ifdef __clang__
    name = __PRETTY_FUNCTION__;
    prefix = "auto type_name() [T = ";
    suffix = "]";
#elif defined(__GNUC__)
    name = __PRETTY_FUNCTION__;
    prefix = "constexpr auto type_name() [with T = ";
    suffix = "]";
#elif defined(_MSC_VER)
    name = __FUNCSIG__;
    prefix = "auto __cdecl type_name<";
    suffix = ">(void)";
#endif
    name.remove_prefix(prefix.size());
    name.remove_suffix(suffix.size());
    return name;
}

// int a = 3;
// const int& b = 4;
// cout << type_name<decltype(a)>() << endl;
// cout << type_name<decltype(b)>() << endl;
// decltype(b) k = 4;
// cout << type_name<decltype(k)>() << endl;


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

#endif

#endif // !TEMPLATE_FUNCTIONS_HPP

