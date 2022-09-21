/******************************************************************************
 * CountingIterator<> is a template class which
 * 
 * 
 * 
 *
 * @Authur :  JeJo
 * @Date   :  January - 2021
 * @license: free to use and distribute(no further support as well)
 *****************************************************************************/

#ifndef JEJO_CountingIterator_T_HPP
#define JEJO_CountingIterator_T_HPP

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
class CountingIterator final
{
private:
    Type mValue;

public:
    using iterator_category = std::input_iterator_tag;
    using value_type = Type;
    using reference = Type;
    using poTypeer = Type*;
    using difference_type = std::ptrdiff_t;


    constexpr explicit CountingIterator(Type x) noexcept
        : mValue{ x } 
    {}
    constexpr CountingIterator(const CountingIterator&) noexcept = default;
    constexpr CountingIterator& operator=(const CountingIterator&) noexcept = default;

    // for forward iteration
    constexpr CountingIterator& operator++() noexcept
    {
        mValue++;
        return *this;
    }

    CountingIterator operator++(int) noexcept
    {
        const auto copy = *this;
        ++(*this);
        return copy;
    }

    constexpr reference operator*() const { return mValue; }

    constexpr bool operator==(const CountingIterator& other) const noexcept
    {
        return mValue == other.mValue;
    }

    constexpr bool operator!=(const CountingIterator& other) const noexcept
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
    
    constexpr bool operator!=(const ReverseCountingIterator& /* other */) const noexcept
    {
        return mValue != -1;
    }
};


/*!
 *
 */
template<typename Type>
class ForwardRangeIota final
{
    Type mBegin, mEnd;
public:
    constexpr explicit ForwardRangeIota(const Type begin, const Type end) noexcept
        : mBegin{ begin }
        , mEnd { end }
    {}

    constexpr auto begin() noexcept
    {
        return CountingIterator<Type>{ mBegin };
    }

    constexpr auto cbegin() const noexcept
    {
        return CountingIterator<Type>{ mBegin };
    }

    constexpr auto end() noexcept
    { 
        return CountingIterator<Type>{ mEnd };
    }

    constexpr auto cend() const noexcept
    {
        return CountingIterator<Type>{ mEnd };
    }
};

/*!
 *
 */
template<typename Type>
class ReverseRangeIota final
{
    Type mBegin, mEnd;

    constexpr auto calc_pos() noexcept
    {
        return ReverseCountingIterator<Type>{
            ((mBegin - 1 != 0) && (mBegin - 1 < mBegin))
                ? mBegin - 1
                : mEnd
        };
    }

public:
    constexpr explicit ReverseRangeIota(const Type begin, const Type end) noexcept
        : mBegin{ begin }
        , mEnd{ end }
    {}

    constexpr auto begin() noexcept
    {
        return calc_pos();
    }

    constexpr auto cbegin() const noexcept
    {
        return calc_pos();
    }

    constexpr auto end() noexcept
    {
        return ReverseCountingIterator<Type>{ 0 };
    }

    constexpr auto cend() const noexcept
    {
        return ReverseCountingIterator<Type>{ 0 };
    }
};

#if 0
// deduction guide for the IotaForwardRange<Type>
template<typename Type> IotaForwardRange(Type)->IotaForwardRange<Type>;

// deduction guide for the IotaReverseRange<Type>
template<typename Type> IotaReverseRange(Type)->IotaReverseRange<Type>;
#endif
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


JEJO_END

#endif // JEJO_CountingIterator_T_HPP

/*****************************************************************************/
