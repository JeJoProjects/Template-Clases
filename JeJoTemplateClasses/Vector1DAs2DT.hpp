#ifndef VECTOR_1D_AS_2D_T_HPP
#define VECTOR_1D_AS_2D_T_HPP

#include <vector>
#include <cstddef>
#include <iostream>
#include <format>

/*!
 * Vector1DAs2D<T> class templates defines basic functionalities for manipulating
 * the one dimensional std::vector internal storage as a 2 dimensional data.
 */
template<typename Type>
class Vector1DAs2D final
{
    class Proxy;
public:
    constexpr explicit Vector1DAs2D(std::size_t row, std::size_t col, Type val = {})
        : mColumn{ col }
    {
        mVector.resize(row * col, val);
    }

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

    /*
     * 1D vector element manupulation via operator()().
     */
    constexpr Type& operator()(std::size_t row, std::size_t col)
    {
        return mVector[(row * mColumn) + col];
    }

    constexpr const Type& operator()(std::size_t row, std::size_t col) const
    {
        return mVector[(row * mColumn) + col];
    }

    void print() const noexcept
    {
        for (auto i = 0u; i < row(); ++i)
        {
            for (auto j = 0u; j < col(); ++j)
            {
                std::cout << std::format("[{}, {}] : {}\t", i, j, this->operator()(i, j));
            }
            std::cout << "\n";
        }
    }

    friend void print2(/*const*/ Vector1DAs2D& ob) noexcept
    {
        for (auto i = 0u; i < ob.row(); ++i)
        {
            for (auto j = 0u; j < ob.col(); ++j)
            {
                std::cout << std::format("[{}, {}] : {}\t", i, j, ob[i][j]);
            }
            std::cout << "\n";
        }
    }

    Proxy& operator[](std::size_t row)
    {
        static Proxy ob{ this };
        ob.setRow(row);
        return ob;
    }

    const Proxy& operator[](std::size_t row) const
    {
        static Proxy ob{ this };
        ob.setRow(row);
        return ob;
    }


private:

    class Proxy final 
    {
    public:
        Proxy(Vector1DAs2D<Type>* _array)
            : _array{ _array }
        {}

        void setRow(std::size_t row) noexcept
        {
            mRow = row;
        }

        constexpr Type& operator[](std::size_t col)
        {
            return _array->operator()(mRow, col);
        }

        constexpr const Type& operator[](std::size_t col) const
        {
            return _array->operator()(mRow, col);
        }

    private:
        Vector1DAs2D<Type>* _array{ nullptr};
        std::size_t mRow{};
    };

private:
    std::vector<Type> mVector;
    std::size_t mColumn{};
};

#endif // VECTOR_1D_AS_2D_T_HPP
