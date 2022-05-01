#ifndef VECTOR_1D_AS_2D_T_HPP
#define VECTOR_1D_AS_2D_T_HPP

#include <vector>
#include <cstddef>
#include <iostream>
#include <format>
#include <functional>
#include <ranges>

// macros for copy-move contrs
#define DISABLE_COPY(Class) \
    Class(const Class&) = delete; \
    Class& operator=(const Class&) = delete

#define DISABLE_MOVE(Class) \
    Class(Class&&) = delete; \
    Class& operator=(Class&&) = delete

#define DISABLE_COPY_MOVE(Class) \
    DISABLE_COPY(Class); DISABLE_MOVE(Class)

#define DEFAULT_COPY(Class) \
    Class(const Class&) = default; \
    Class& operator=(const Class&) = default

#define DISABLE_MOVE(Class) \
    Class(Class&&) = delete; \
    Class& operator=(Class&&) = delete

#define DEFAULT_MOVE(Class) \
    Class(Class&&) = default; \
    Class& operator=(Class&&) = default

#define DEFAULT_COPY_MOVE(Class) \
    DEFAULT_COPY(Class); DEFAULT_MOVE(Class)

// concept for class template instantiation
template<typename Type>
concept IsPointer = std::is_pointer_v<Type>;

/*!
 * Vector1DAs2D<T> class templates defines basic functionalities for manipulating
 * the one dimensional std::vector internal storage as a 2 dimensional data.
 */
template<typename Type>
class Vector1DAs2D final
{
private:
    // forward declaration(s)
    template<IsPointer T> class Proxy;

    // type aliase(s)
    using ProxyClassPtr = Proxy<Vector1DAs2D<Type>*>;
    using ProxyConstClassPtr = Proxy<const Vector1DAs2D<Type>*>;

public:
    // @TODO: types and helper types
    using value_type = Type;

    constexpr explicit Vector1DAs2D(std::size_t row, std::size_t col, Type val = {})
        : mColumn{ col }
    {
        mVector.resize(row * col, val);
    }

    constexpr auto begin() noexcept { return std::begin(mVector); }
    constexpr auto end() noexcept { return std::end(mVector); }
    constexpr auto cbegin() const noexcept { return std::cbegin(mVector); }
    constexpr auto cend() const noexcept { return std::cend(mVector); }

    // other special member functions: if required!
    template<typename InputIt>
    constexpr void assign(InputIt first, InputIt last)
    {
        mVector.assign(first, last);
    }

    constexpr auto width() const noexcept { return mColumn; }
    constexpr auto col() const noexcept { return this->width(); }

    constexpr auto height() const noexcept { return mVector.size() / mColumn; }
    constexpr auto row() const noexcept { return this->height(); }

    ProxyClassPtr& operator[](std::size_t row)
    {
        static ProxyClassPtr ob{ this };
        ob.setRow(row);
        return  ob;
    }


    const ProxyConstClassPtr& operator[](std::size_t row) const
    {
        static ProxyConstClassPtr ob{ this };
        ob.setRow(row);
        return ob;
    }

    void print() const noexcept
    {
        auto idx = 0ull;
        // Note: Not goot the "const_cast", however for fun!
        for (const Type ele : const_cast<Vector1DAs2D<Type>&>(*this))
        {
            std::cout << ele << " ";
            if (++idx % this->col() == 0)
                std::cout << "\n";
        }
    }

    friend std::ostream& operator<<(std::ostream& out,  Vector1DAs2D& ob)
    {
        for (const std::size_t i : std::views::iota(0ull, ob.row()))
        {
            for (const std::size_t j : std::views::iota(0ull, ob.col()))
            {
                out << std::format("[{}, {}] : {}\t", i, j, ob[i][j]);
            }
            out << "\n";
        }
        return out;
    }

private:
    /*!
     * private internal Proxy class for providing the operator[][] for the
     * Vector1DAs2D class.
     */
    template<IsPointer T> class Proxy final
    {
    public:
        explicit constexpr Proxy(T arrPtr) noexcept
            : mArrayPtr{ arrPtr }
        {}

        // disabled the copy and move as not required!
        DISABLE_COPY_MOVE(Proxy);

        constexpr void setRow(std::size_t row) noexcept
        {
            mRow = row;
        }

        constexpr Type& operator[](std::size_t col)
        {
            return mArrayPtr->operator()(mRow, col);
        }

        constexpr const Type& operator[](std::size_t col) const
        {
            return mArrayPtr->operator()(mRow, col);
        }

    private:
        T mArrayPtr{ nullptr };
        std::size_t mRow{};
    };

    /*!
     * 1D vector element manipulation via operator()().
     */
#if 1
    constexpr Type& operator()(std::size_t row, std::size_t col)
    {
        return mVector[(row * mColumn) + col];
    }

    constexpr const Type& operator()(std::size_t row, std::size_t col) const
    {
        return mVector[(row * mColumn) + col];
    }
#elif 0 //  deducing "this": not yet but --> coming soon!
    template<typename Self>
    decltype(auto) name(this Self&& self)
    {
        return std::forward<Self>(self).mVector;
    }
#endif

private:
    std::vector<Type> mVector;
    std::size_t mColumn{};
};

#endif // VECTOR_1D_AS_2D_T_HPP

