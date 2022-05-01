/******************************************************************************
 * Implementation of Fraction<>, which provides a template class, to show
 * the decimals in a fractional form of it.
 * Currently the class is capable of handling integer (template)parameters.
 *
 * @Authur :  JeJo
 * @Date   :  June - 2018
 * @license: free to use and distribute(no further support as well)
 *****************************************************************************/

#ifndef JEJO_FRACTION_T_HPP
#define JEJO_FRACTION_T_HPP

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// C++ headers
#include <iostream>
#include <cmath>       // std::abs
#include <algorithm>   // std::min
#include <utility>     // std::move
#include <iomanip>     // std::setw
#include <type_traits> // std::is_integral<>, std::is_same<>, std::enable_if<>

JEJO_BEGIN

// traits for possible template types(s)
template<typename Type>
using is_allowed_integral = std::conjunction<
    std::is_integral<Type>,
    std::negation<std::is_same<Type, bool>>,
    std::negation<std::is_same<Type, char>>,
    std::negation<std::is_same<Type, char16_t>>,
    std::negation<std::is_same<Type, char32_t>>,
    std::negation<std::is_same<Type, wchar_t>> >;
  /*std::negation<std::is_same<char8_t, Type>> */ // valid since C++20
   /*|| std::is_floating_point<Type>::value*/
   // @todo: should be also available for floats ?

template<typename Type> using is_allowed_integral_t
   = std::enable_if_t<JeJo::is_allowed_integral<Type>::value>;

/* conditional instantiation of the template class, depending on the template
 * argument.
 */
template<typename Type, typename Enable = void> class Fraction;

// forward declaration
template<typename U>
std::ostream& operator<<(std::ostream& out, const Fraction<U>& f) noexcept;

template<typename Type>
class Fraction<Type, JeJo::is_allowed_integral_t<Type>> final
{
private:
	// internal class
	class DividerHelper final
	{
	private:
		Type _numerator, _denominator;

	private:
		// internal function(s)
		void simplificate() noexcept
		{
			Type commondivisor = 1;
			const Type n = std::min(
				std::abs(_numerator), std::abs(_denominator));
			for (Type i = 2; i <=n ; i++)
				if (_numerator % i == 0 && _denominator % i == 0)
					commondivisor = i;
			_numerator /= commondivisor;
			_denominator /= commondivisor;
		}

	public:
		// constructor(s)
		constexpr explicit DividerHelper(
			const Type num, const Type den = 1) noexcept
			:_numerator{ num }, _denominator{ den }
		{}

		// copy : disabled
		DividerHelper(const DividerHelper& other) = delete;
		DividerHelper& operator= (DividerHelper const& other) = delete;

		// move : enabled
		constexpr DividerHelper(DividerHelper &&other) noexcept = default;
		constexpr DividerHelper& operator= (
			DividerHelper &&other) noexcept = default;

		// getter(s)
		constexpr Type numerator() const noexcept { return this->_numerator; }
		constexpr Type denominator()const noexcept{return this->_denominator;}

		// overloaded operator/=
		DividerHelper& operator/= (const DividerHelper &divisor) noexcept
		{
			this->_numerator *= divisor._denominator;
			this->_denominator *= divisor._numerator;
			this->simplificate();
			return *this;
		}

		// non-member function(s)
		friend DividerHelper operator/ (
			DividerHelper& A, const DividerHelper& B) noexcept
		{
			return std::move(A /= B);
		}
	};

private:
	// DividerHelper doesn't required a default constructor any more!
	DividerHelper _fraction{ 1 }; // default initialization

public:
	// public type alias for friend functions.
	using DividerHelper = Fraction::DividerHelper;

	// constructor(s)
	constexpr explicit Fraction(const Type num, const Type den) noexcept
	{
		DividerHelper _numPart{ num };
		const DividerHelper _denPart{ den };
		this->_fraction = std::move(_numPart / _denPart);
	}

	// copy : disabled
	Fraction(const Fraction &other) = delete;
	Fraction& operator=(const Fraction &other) = delete;

	// move : enabled
	constexpr Fraction(const Fraction &&other) noexcept
		: _fraction{ std::move(other._fraction) }
	{}
	constexpr Fraction& operator= (const Fraction &&other) noexcept
	{
		this->_fraction = std::move(other._fraction);
		return *this;
	}

	// getter(s)
	constexpr Type numerator() const noexcept {
		return this->_fraction.numerator();
	}
	constexpr Type denominator() const noexcept {
		return this->_fraction.denominator();
	}

	constexpr double getReal() const noexcept
	{
		return  static_cast<double>(this->numerator()) /
			static_cast<double>(this->denominator());
	}

	// extended functionality: specialization of operator<< for template "U".
	template<typename U>
	friend std::ostream& operator<< (
		std::ostream& out, const Fraction<U>& f) noexcept
    {
	    return f.numerator() && f.denominator() && f.denominator() != 1 ?
			out << f.numerator() << '/' << f.denominator():
		f.numerator() && f.denominator() == 1 ? out << f.numerator() :
		f.numerator() == f.denominator() ? out << "1" :
		f.numerator() && !f.denominator() ? out << "Infinity" : out << "0";
    }
};



JEJO_END

#endif // JEJO_FRACTION_T_HPP

/*****************************************************************************/
