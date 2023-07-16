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

#if  __cplusplus <= 201703L

// Overload - 1 : Default Case!
// When iterateAsIntegrals == False ---> return CountingRange<T>.
// 
// Overload - 2
// When iterateAsIntegrals == False, and
// T == Enum ---> return CountingRange<typename std::underlying_type<T>::type>.
//
// Overload - 3
// When iterateAsIntegrals == True, and
// T == Int ---> return CountingRange<T>.
//
// Overload - 4
// When iterateAsIntegrals = False, and
// T and U == int or enum ---> Iterate an an Enum!
template<bool iterateAsIntegrals = false, typename T = int>
inline constexpr auto makeRange(T first, T last)
{
    if constexpr (!iterateAsIntegrals)
    {
        return CountingRange<T>{ first, last };
    }
    else if constexpr (iterateAsIntegrals && std::is_enum_v<T>)
    {
        using UnderlyingType = typename std::underlying_type<T>::type;

        return CountingRange<UnderlyingType>{ static_cast<UnderlyingType>(first),
            static_cast<UnderlyingType>(last) };
    }
    else if constexpr (iterateAsIntegrals && std::is_integral_v<T>)
    {
        return makeRange(first, last);
    }
}

// Overload - 4
// When iterateAsIntegrals = False, and
// T and U == int or enum ---> Iterate an an Enum!
// 
// Overload - 5
// When iterateAsIntegrals = True, and
// T and U == int or enum ---> Iterate an an Inger!
template<bool iterateAsIntegrals = false, typename T = int, typename U = int>
inline constexpr auto makeRange(T first, U last)
{
    if constexpr ((std::is_integral_v<T> || std::is_enum_v<T>)
        && (std::is_integral_v<U> || std::is_enum_v<U>)) 
    {
        if constexpr (iterateAsIntegrals)
        {
            using ReType = std::conditional_t<std::is_enum_v<T>&& std::is_integral_v<U>, U, T>;
            return makeRange(static_cast<ReType>(first), static_cast<ReType>(last));
        }
        else if constexpr (!iterateAsIntegrals)
        {
            using ReType = std::conditional_t<std::is_enum_v<T>&& std::is_integral_v<U>, T, U>;
            return makeRange(static_cast<ReType>(first), static_cast<ReType>(last));
        }

    }
}

#elif __cplusplus <= 201103L

// Overload - 1 : Default Case!
// When iterateAsIntegrals == False ---> return CountingRange<T>.
template<bool iterateAsIntegrals = false, typename T = int>
inline constexpr auto makeRange(T first, T last)
-> std::enable_if_t<iterateAsIntegrals == false, CountingRange<T>>
{
    return CountingRange<T>{ first, last };
}


// Overload - 2
// When iterateAsIntegrals == False, and
// T == Enum ---> return CountingRange<typename std::underlying_type<T>::type>.
template<bool iterateAsIntegrals = false, typename T = int>
inline constexpr auto makeRange(T first, T last)
-> std::enable_if_t<iterateAsIntegrals == true && std::is_enum_v<T>, CountingRange<typename std::underlying_type<T>::type>>
{
    using UnderlyingType = typename std::underlying_type<T>::type;

    return CountingRange<UnderlyingType>{ static_cast<UnderlyingType>(first),
        static_cast<UnderlyingType>(last) };
}


// Overload - 3
// When iterateAsIntegrals == True, and
// T == Int ---> return CountingRange<T>.
template<bool iterateAsIntegrals = false, typename T = int>
inline constexpr auto makeRange(T first, T last)
-> std::enable_if_t<iterateAsIntegrals == true && std::is_integral_v<T>, CountingRange<T>>
{
    return makeRange(first, last);
}


// Overload - 4
// When iterateAsIntegrals = False, and
// T and U == int or enum ---> Iterate an an Enum!
template<bool iterateAsIntegrals = false, typename T = int, typename U = int>
inline constexpr auto makeRange(T first, U last)
-> std::enable_if_t<
    iterateAsIntegrals == false
    && (std::is_integral_v<T> || std::is_enum_v<T>)
    && (std::is_integral_v<U> || std::is_enum_v<U>)
    , CountingRange<std::conditional_t<std::is_enum_v<T>&& std::is_integral_v<U>, T, U>>>
{
    using ReType = std::conditional_t<std::is_enum_v<T>&& std::is_integral_v<U>, T, U>;
    return makeRange(static_cast<ReType>(first), static_cast<ReType>(last));
}


// Overload - 5
// When iterateAsIntegrals = True, and
// T and U == int or enum ---> Iterate an an Inger!
template<bool iterateAsIntegrals = false, typename T = int, typename U = int>
inline constexpr auto makeRange(T first, U last)
-> std::enable_if_t<
    iterateAsIntegrals == true
    && (std::is_integral_v<T> || std::is_enum_v<T>)
    && (std::is_integral_v<U> || std::is_enum_v<U>)
    , CountingRange<std::conditional_t<std::is_enum_v<T>&& std::is_integral_v<U>, U, T>>>
{
    using ReType = std::conditional_t<std::is_enum_v<T>&& std::is_integral_v<U>, U, T>;
    return makeRange(static_cast<ReType>(first), static_cast<ReType>(last));
}

#endif 



JEJO_END

#endif // JEJO_CountingRange_T_HPP

/*****************************************************************************/
