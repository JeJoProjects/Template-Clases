/******************************************************************************
 * ForwardCountingIterator<> is a template class which
 * 
 * 
 * 
 *
 * @Authur :  JeJo
 * @Date   :  January - 2021
 * @license: free to use and distribute(no further support as well)
 *****************************************************************************/

#ifndef JEJO_ForwardCountingIterator_T_HPP
#define JEJO_ForwardCountingIterator_T_HPP

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// C++ headers
#include <iostream>
#include <mutex> // std::once_flag

JEJO_BEGIN

// Simplified and lightweight version of std::views::iota (from C++20)
// https://stackoverflow.com/questions/63399816/


/*!
 * The light weight FORWARD iterator for counting the integer from the rage [0, Value)
 */
template<typename Type>
class ForwardCountingIterator final
{
private:
    Type mValue;

public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Type;
    using reference = Type;
    using poTypeer = Type*;
    using difference_type = std::ptrdiff_t;


    constexpr explicit ForwardCountingIterator(Type x)noexcept
        : mValue{ x } 
    {}
    constexpr ForwardCountingIterator(const ForwardCountingIterator&) noexcept = default;
    constexpr ForwardCountingIterator& operator=(const ForwardCountingIterator&) noexcept = default;
    constexpr reference operator*() const { return mValue; }

    constexpr ForwardCountingIterator& operator++() noexcept
    {
        mValue++;
        return *this;
    }

    ForwardCountingIterator operator++(int) noexcept
    {
        const auto copy = *this;
        ++(*this);
        return copy;
    }

    constexpr bool operator==(const ForwardCountingIterator& other) const noexcept
    {
        return mValue == other.mValue;
    }

    constexpr bool operator!=(const ForwardCountingIterator& other) const noexcept
    {
        return mValue != other.mValue;
    }
};

/*!
 * The light weight BACKWARD/ REVERSE iterator for counting the integer
 * from the rage (Value, 0]
 */
template<typename Type>
class ReverseCountingIterator final
{
private:
    Type mValue;

public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Type;
    using reference = Type;
    using poTypeer = Type*;
    using difference_type = std::ptrdiff_t;

    constexpr explicit ReverseCountingIterator(Type x)
        : mValue{ x }
    {}

    constexpr ReverseCountingIterator(const ReverseCountingIterator&) noexcept = default;
    constexpr ReverseCountingIterator& operator=(const ReverseCountingIterator&) noexcept = default;
    constexpr reference operator*() const { return mValue; }

    ReverseCountingIterator& operator++() noexcept
    {
        --mValue;
        return *this;
    }

    ReverseCountingIterator operator++(int) noexcept
    {
        const auto copy = *this;
        --mValue;
        return copy;
    }

    constexpr bool operator==(const ReverseCountingIterator& other) const noexcept
    {
        return mValue == other.mValue;
    }
    
    constexpr bool operator!=(const ReverseCountingIterator& other) const noexcept
    {
        return mValue != -1;
    }
};


/*!
 *
 */
template<typename Type>
class IotaForwardRange final
{
    Type mLast;

public:
    constexpr explicit IotaForwardRange(const Type last) noexcept
        : mLast{ last }
    {}

    constexpr ForwardCountingIterator<Type> begin() const noexcept
    {
        return ForwardCountingIterator<Type>{ 0 };
    }

    constexpr ForwardCountingIterator<Type> end() const noexcept
    { 
        return ForwardCountingIterator<Type>{ mLast }; 
    }
};

/*!
 *
 */
template<typename Type>
class IotaReverseRange final
{
    Type mLast;

public:
    constexpr explicit IotaReverseRange(const Type last) noexcept
        : mLast{ last }
    {}

    constexpr ReverseCountingIterator<Type> begin() const noexcept
    {
        return ReverseCountingIterator<Type>{ 
            ((mLast - 1 != 0) && (mLast - 1 < mLast))
                ? mLast - 1
                : 0
        };
    }

    constexpr ReverseCountingIterator<Type> end() const noexcept
    {
        return ReverseCountingIterator<Type>{ 0 };
    }
};

// deduction guide for the IotaForwardRange<Type>
template<typename Type> IotaForwardRange(Type)->IotaForwardRange<Type>;

// deduction guide for the IotaReverseRange<Type>
template<typename Type> IotaReverseRange(Type)->IotaReverseRange<Type>;

using int32 = unsigned int;


/*!
 * Dummy array<> class for test
 */
template<typename T> struct TArray : public std::vector<T>
{
    using std::vector<T>::vector;

    int32 size() const noexcept
    {
        return static_cast<int32>(std::vector<T>::size());
    }
};

// flag to activate/deactivate the code snippet
#define FLAG 0

template<typename Type>
constexpr void testForward(Type last) noexcept
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
constexpr void testReverse(Type last)  noexcept
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

JEJO_END

#endif // JEJO_ForwardCountingIterator_T_HPP

/*****************************************************************************/
