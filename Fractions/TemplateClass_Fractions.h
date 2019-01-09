
#include <iostream>
#include <algorithm>

class Rational
{
private:
    int m_numerator, m_denominator;
private:
    inline void simplificate()
    {
        int commondivisor = 1;
        for(int i=2;i<= std::min(abs(m_numerator), abs(m_denominator));i++)
            if( m_numerator%i == 0 && m_denominator%i == 0 )
                commondivisor = i;
        m_numerator /= commondivisor;
        m_denominator /= commondivisor;
    }
    /*inline void swap(Rational& other)
    {
        std::swap(m_numerator, other.m_denominator);
        std::swap(m_denominator, other.m_denominator);
    }*/
public:
    
    Rational()                                  // Defualt
        :m_numerator(1), m_denominator(1)
        {}

    Rational(const int& num, const int& den=1)  // Parameterized
        :m_numerator(num), m_denominator(den)
        {}

    Rational(const Rational& other)             // Copy
        :m_numerator(other.m_numerator), m_denominator(other.m_denominator)
        {}

    Rational(Rational&& other)                  // Move
        :m_numerator(other.m_numerator), m_denominator(other.m_denominator)
        {}
    /*Rational(Rational&& other)                // another Move
        :m_numerator(0), m_denominator(0)
        {   swap(other);    }*/

    ~Rational(){}

    Rational& operator= (Rational const& other) = default;
    Rational& operator/ (const int& divisor)
    {
            m_denominator *= divisor;
            simplificate();
            return *this;
    }
    Rational& operator/= (const Rational &divisor)
    {
        m_numerator *= divisor.m_denominator;
        m_denominator *= divisor.m_numerator;
        simplificate();
        return *this;
    }
    const double getrealformat()const
    {
        return  static_cast<double>(m_numerator)/
                static_cast<double>(m_denominator);
    }

    friend double operator/ (Rational& obj, const int& divisor);
    friend Rational operator/ (Rational A, Rational const & B);
    friend void printRational(Rational& obj, const int& A, const int& B);
};
/************************** Friend functions ********************************/
Rational operator/ (Rational A, Rational const& B)
{
    return std::move(A/=B);
}
double operator/ (Rational& obj, const int& divisor)
{
    obj.m_denominator *= divisor;
    obj.simplificate();
    return obj.getrealformat();
}
void printRational(Rational& obj, const int& A, const int& B)   // lvalue
{
    Rational r1(A), r2(B);
    obj = r1/r2;
    std::cout<<obj.m_numerator<<'/'<<obj.m_denominator<<std::endl;
    //std::cout<<obj.getrealformat()<<std::endl;
}
/*****************************************************************************/
