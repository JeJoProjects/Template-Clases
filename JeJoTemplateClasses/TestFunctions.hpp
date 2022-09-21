#ifndef TEST_FUNCTIONS_HPP
#define TEST_FUNCTIONS_HPP

#include <vector>
#include "ForwardCountingIterator.hpp"


// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

JEJO_BEGIN

// ---------- SFINAE_HPP ---------



// ---------- TEMPLATE_FUNCTION_HPP ---------
#pragma region template_functions 

#pragma endregion

// ---------- TRAITS_HPP ---------
#pragma region traits 

#pragma endregion 


// ---------- TEMPLATE_CLASSES_HPP ---------
#pragma region template_classes


// flag to activate/deactivate the code snippet
#define FLAG 0
template<typename Type>
constexpr void testForward(Type last) noexcept;

template<typename Type>
constexpr void testReverse(Type last)  noexcept;

void static_variant_test();


#pragma endregion


template<typename Type>
inline constexpr void testForward(Type last) noexcept
{
    std::once_flag flag;

    for (const auto index : IotaForwardRange{ last })
    {
#if FLAG

        std::call_once(flag, [](auto i) constexpr noexcept
            {
                std::cout << "Type of ArraySize() = type of the index: " << std::boolalpha
                    << std::is_same_v<Type, decltype(i)>
                    << '\n';

            }, index);
#endif
        std::cout << index << " ";
    }
    std::cout << '\n';
}

template<typename Type>
inline constexpr void testReverse(Type last) noexcept
{
    std::once_flag flag;
    for (const auto index : IotaReverseRange{ last })
    {
#if FLAG
        std::call_once(flag, [](auto i) constexpr noexcept
            {
                std::cout << "Type of ArraySize() = type of the index: " << std::boolalpha
                    << std::is_same_v<Type, decltype(i)>
                    << '\n';
            }, index);
#endif
        std::cout << index << " ";
    }
    std::cout << '\n';
}
JEJO_END // namespace JeJo

#endif // !TEST_FUNCTIONS_HPP
