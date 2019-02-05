#include <cmath>
#include <algorithm>
#include <utility>
#include <type_traits>

template<typename Type>
constexpr bool  is_okay_type = std::is_integral_v<Type> || std::is_floating_point_v<Type>;

template<typename Type = int>
class Fraction final
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
			for (Type i = 2; i <= std::min(std::abs(_numerator), std::abs(_denominator)); i++)
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
			_numerator *= divisor._denominator;
			_denominator *= divisor._numerator;
			simplificate();
			return *this;
		}

		friend DividerHelper operator/ (DividerHelper &A, const DividerHelper &B) noexcept
		{			
			return std::move(A /= B);
		}
	};

private:
	DividerHelper _fraction;

public:
	explicit Fraction(const Type num, const Type den) noexcept
	{
		DividerHelper _numPart{ num };
		const DividerHelper _denPart{ den };
		this->_fraction = std::move(_numPart /= _denPart);
	}

	// getter
	Type numerator() const noexcept { return this->_fraction.numerator(); }
	Type denominator() const noexcept { return this->_fraction.denominator(); }

	double getReal() const noexcept
	{
		return  static_cast<double>(this->numerator()) / 
			    static_cast<double>(this->denominator());
	}
};

template<typename Type>
std::ostream& operator<<(std::ostream& out, const Fraction<Type>& f) noexcept
{
	return out << f.numerator() << '/' << f.denominator() << " ";
}