#include <cmath>
#include <algorithm>
#include <utility>

class Fraction final
{
private:
	class DividerHelper final
	{
	private:
		int _numerator, _denominator;
	private:
		void simplificate()
		{
			int commondivisor = 1;
			for (int i = 2; i <= std::min(std::abs(_numerator), std::abs(_denominator)); i++)
				if (_numerator%i == 0 && _denominator%i == 0)
					commondivisor = i;
			_numerator /= commondivisor;
			_denominator /= commondivisor;
		}

	public:
		DividerHelper() = default;
		DividerHelper(const int num, const int den = 1)
			:_numerator(num), _denominator(den)
		{}

		// Copy
		DividerHelper(const DividerHelper& other) = delete;
		DividerHelper& operator= (DividerHelper const& other) = delete;
		// Move
		DividerHelper(DividerHelper &&other) = default;
		DividerHelper& operator= (DividerHelper &&other) = default;
		// getter
		int numerator() const noexcept { return this->_numerator; }
		int denominator() const noexcept { return this->_denominator; }

		DividerHelper& operator/ (const int divisor)
		{
			_denominator *= divisor;
			simplificate();
			return *this;
		}

		DividerHelper& operator/= (const DividerHelper &divisor)
		{
			_numerator *= divisor._denominator;
			_denominator *= divisor._numerator;
			simplificate();
			return *this;
		}

		const double getrealformat()const
		{
			return  static_cast<double>(_numerator) /
				static_cast<double>(_denominator);
		}
		friend DividerHelper operator/ (DividerHelper A, DividerHelper B);
	};
#if 1
	DividerHelper operator/ (DividerHelper A, DividerHelper B)
	{
		A = std::move(A / B);
		return A;
	}
#endif
private:
	DividerHelper _fraction;

public:
	explicit Fraction(const int num, const int den)
	{
		DividerHelper _numPart{ num };
		DividerHelper _denPart{ den };
		_fraction = std::move(_numPart /= _denPart);
	}

	// getter
	int numerator() const noexcept { return this->_fraction.numerator(); }
	int denominator() const noexcept { return this->_fraction.denominator(); }

};

std::ostream& operator<<(std::ostream& out, const Fraction& f) noexcept
{
	return out << f.numerator() << '/' << f.denominator() << " ";
}