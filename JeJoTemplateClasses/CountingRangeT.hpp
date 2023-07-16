/******************************************************************************
 * CountingRange<> is a template class which provide a range for iterating through.
 *
 *
 *
 *
 * @Authur :  JeJo
 * @Date   :  March - 2023
 * @license: free to use and distribute(no further support as well)
 *****************************************************************************/

#ifndef JEJO_CountingRange_T_HPP
#define JEJO_CountingRange_T_HPP

 // C++ headers
#include <iterator>
#include <cassert>

 // macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }


JEJO_BEGIN

template <typename T>
class CountingRange final
{
public:
    // Iterator traits
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    constexpr  explicit CountingRange(T current, T end) noexcept
        : mStart{ current }
        , mEnd{ end }
    {
        assert(mStart <= mEnd);
    }
    constexpr CountingRange(const CountingRange&) = default;
    CountingRange& operator=(const CountingRange&) = default;
    constexpr CountingRange(CountingRange&&) noexcept = default;
    CountingRange& operator=(CountingRange&&) noexcept = default;
    ~CountingRange() = default;


    constexpr reference operator*() const noexcept {
        return mStart;
    }

    constexpr pointer operator->() const noexcept {
        return &mStart;
    }

    CountingRange& operator++() {
        IncrementHelper(mStart);
        return *this;
    }

    constexpr CountingRange operator++(int) {
        auto temp = *this;
        IncrementHelper(mStart);
        return temp;
    }

    constexpr bool operator==(const CountingRange& other) const noexcept {
        return mStart == other.mStart;
    }

    constexpr bool operator!=(const CountingRange& other) const noexcept {
        return !(*this == other);
    }

    constexpr CountingRange begin() const {
        return *this;
    }

    constexpr CountingRange end() const {
        return CountingRange{ mEnd, mEnd };
    }

private:
    // Helper function for incrementing enum class values
    template <typename E>
    typename std::enable_if<std::is_enum<E>::value>::type IncrementHelper(E& value)
    {
        using UnderlyingType = typename std::underlying_type<E>::type;

        value = static_cast<E>(static_cast<UnderlyingType>(value) + static_cast<UnderlyingType>(1));
    }

    // Helper function for incrementing non-enum class values
    template <typename E>
    typename std::enable_if<!std::is_enum<E>::value>::type IncrementHelper(E& value)
    {
        ++value;
    }

private:
    T mStart;
    T mEnd;
};

template<bool iterateAsIntegrals = false, typename T = int>
constexpr auto makeRange(T first, T last)
-> std::enable_if_t<iterateAsIntegrals == false, CountingRange<T>>
{
    return CountingRange<T>{ first, last };
}

template<bool iterateAsIntegrals = false, typename T = int>
constexpr auto makeRange(T first, T last)
-> std::enable_if_t<iterateAsIntegrals == true && std::is_enum_v<T>, CountingRange<typename std::underlying_type<T>::type>>
{
    using UnderlyingType = typename std::underlying_type<T>::type;

    return CountingRange<UnderlyingType>{ static_cast<UnderlyingType>(first), 
        static_cast<UnderlyingType>(last) };
}

template<bool iterateAsIntegrals = false, typename T = int>
constexpr auto makeRange(T first, T last)
-> std::enable_if_t<iterateAsIntegrals == true && std::is_integral_v<T>, CountingRange<T>>
{
    return makeRange(first, last);
}



JEJO_END

#endif // JEJO_CountingRange_T_HPP

/*****************************************************************************/
