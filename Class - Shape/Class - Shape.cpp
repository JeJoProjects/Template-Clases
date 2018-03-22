#include <iostream>

/******************** Base Object - Abstract *********************************/
template<class T>
class Shape
{
public:
    virtual const T getPerimeter()const =0;
    virtual const T getArea()const =0;
};
/****************** Derived: TRIANGLE - Abstract ******************************/
template<class T> // @not @complete
class Triangle: public Shape<T>
{
private:
    T m_base;
    T m_height;
    T m_A, m_B, m_C;
private:
    inline const T areaCalculator()const
    {
        if(  (m_A == 0.0f) && (m_B == 0.0f) && (m_C == 0.0f) )
        {
            return 0.5*static_cast<T>(m_base)*static_cast<T>(m_height);
        }
        else
        {
            return 0.0;// need to implement this
        }
    }
    inline const T perimeterCalculator()const
    {
        if(  (m_A == 0.0f) && (m_B == 0.0f) && (m_C == 0.0f) )
        {
            return 0.0;// need to implement this
        }
        else
        {
            return m_A+m_B+m_C;
        }
    }
public:
    Triangle()
        :m_base(0.0f), m_height(0.0f),m_A(0.0f), m_B(0.0f), m_C(0.0f) {}
    Triangle(const T& b, const T& h)
        :m_base(b), m_height(h),m_A(0.0f), m_B(0.0f), m_C(0.0f) {}
    Triangle(const T& a, const T& b, const T& c)
        :m_base(0.0f), m_height(0.0f),m_A(a), m_B(b), m_C(c) {}
    ~Triangle() {}

    const T& getTriangleBase()const {   return m_base;  }
    const T& getTriangleHeight()const {   return m_height;  }

    const T getPerimeter()const override{   return perimeterCalculator();  }
    const T getArea()const  override{   return areaCalculator(); }
};
/****************** Derived: CIRCLE - Abstract ******************************/
template<class T>
class Circle: public Shape<T>
{
private:
    T m_radius;
    const T PI = 3.1415926535897932384626433832795;
public:
    Circle():m_radius(0.0f) {}
    Circle(const T& r):m_radius(r) {}
    ~Circle() {}

    const T& getCircleRadius()const {   return m_radius;  }

    const T getPerimeter()const override{   return 2.0*PI*m_radius;  }
    const T getArea()const      override{   return  PI*m_radius*m_radius; }
};
/****************** Derived: RECTANGLE - Abstract ******************************/
template<class T>
class Rectangle: public Shape<T>
{
private:
    T m_length;
    T m_width;
public:
    Rectangle():m_length(0.0f), m_width(0.0f) {}
    Rectangle(const T& l, const T& w):m_length(l), m_width(w) {}
    ~Rectangle() {}
    const T& getRectLength()const{   return m_length; }
    const T& getRectWidth()const {   return m_width;  }
    const T getPerimeter()const override{   return 2.0*(m_length+m_width);  }
    const T getArea()const      override{   return m_length*m_width; }
};
/** ****************************************************************************/
int main()
{
    Shape<float> *shapes[4];

	shapes[0] = new Rectangle<float>(10.0f, 15.0f);
	shapes[1] = new Circle<float>(5.0f);
	shapes[2] = new Triangle<float>(3, 4, 5.2);
	shapes[3] = new Triangle<float>(4, 2.0125);

	for (int n = 0; n < 4; n++)
		std::cout << "Shape[" << n << "] P="
                  << shapes[n]->getPerimeter() << "\t A="
                  << shapes[n]->getArea() << std::endl;

    delete[] shapes;

    return 0;
}
