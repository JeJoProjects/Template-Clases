/******************************************************************************
 * Implementation of Fraction<>, which provides a template class, to show
 * the decimals in a fractional form of it.
 * Currently the class is capable of handling integer parameters(inputs).
 *
 * @Authur :  JeJo
 * @Date   :  June - 2018
 * @license: free to use and distribute(no further support as well)
 *****************************************************************************/

#ifndef JEJO_FRACTION_T_H
#define JEJO_FRACTION_T_H

 // macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

#include <cmath>
#include <algorithm>
#include <utility>     // std::move
#include <type_traits> // is_integral<>

// convenience type
template<typename Type>
inline constexpr bool  is_okay_type =
!std::is_same<bool, Type>::value && !std::is_same<wchar_t, Type>::value
&& std::is_same<char, Type>::value
/*&& !std::is_integral<char8_t, Type>::value */ // valid since C++20
/*|| std::is_floating_point<Type>::value*/;
// @todo: should be also available for floats ?

// convenience type
template<typename Type>
using enable_if_t = typename std::enable_if<is_okay_type<Type>>::type;


template<typename Type, typename Enable = void> class Fraction;


// forward declaration
template<typename Type>
std::ostream& operator<<(std::ostream& out, const Fraction<Type>& f) noexcept;

template<typename Type>
class Fraction<Type, enable_if_t<Type> > final
{
private:
	// internal class
	class DividerHelper final
	{
	private:
		Type _numerator, _denominator;

	private:
		void simplificate() noexcept
		{
			Type commondivisor = 1;
			const Type n = std::min(std::abs(_numerator), std::abs(_denominator));
			for (int i = 2; i <= n; i++)
				if (_numerator%i == 0 && _denominator%i == 0)
					commondivisor = i;
			_numerator /= commondivisor;
			_denominator /= commondivisor;
		}

	public:
		// constructors
		DividerHelper() = default;
		DividerHelper(const Type num, const Type den = 1)
			:_numerator(num), _denominator(den)
		{}

		// Copy : disabled
		DividerHelper(const DividerHelper& other) = delete;
		DividerHelper& operator= (DividerHelper const& other) = delete;

		// Move : enabled
		DividerHelper(DividerHelper &&other) = default;
		DividerHelper& operator= (DividerHelper &&other) = default;

		// getter
		Type numerator() const noexcept { return this->_numerator; }
		Type denominator() const noexcept { return this->_denominator; }

		DividerHelper& operator/= (const DividerHelper &divisor) noexcept
		{
			this->_numerator *= divisor._denominator;
			this->_denominator *= divisor._numerator;
			this->simplificate();
			return *this;
		}

		friend DividerHelper operator/ (
			DividerHelper &A, const DividerHelper &B) noexcept;
	};

private:
	DividerHelper _fraction;

public:
	using DividerHelper = Fraction::DividerHelper;

	explicit Fraction(const Type num, const Type den)
	{
		DividerHelper _numPart{ num };
		const DividerHelper _denPart{ den };
		this->_fraction = std::move(_numPart / _denPart);
	}

	// getter
	Type numerator() const noexcept { return this->_fraction.numerator(); }
	Type denominator() const noexcept { return this->_fraction.denominator(); }

	double getReal() const noexcept
	{
		return  static_cast<double>(this->numerator()) /
			static_cast<double>(this->denominator());
	}
	// extended functionality: specialization of operator<< for template "Type".
	template<typename U>
	friend std::ostream& operator<< <>(std::ostream& out, const Fraction<U>& f) noexcept;
};

// convenience type
template<typename Type> using DividerHelper = typename Fraction<Type>::DividerHelper;

// definition of non-member function
template<typename Type>
DividerHelper<Type> operator/ (DividerHelper<Type> &A, const DividerHelper<Type> &B) noexcept
{
	return std::move(A /= B);
}

// definition of non-member function
template<typename Type>
std::ostream& operator<<(std::ostream& out, const Fraction<Type>& f) noexcept
{
	return out << f.numerator() << '/' << f.denominator() << " ";
}

#endif // JEJO_FRACTION_T_H

/*****************************************************************************/