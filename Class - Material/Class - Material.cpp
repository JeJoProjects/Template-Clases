#include <iostream>
#include <string>

class Material
{
private:
    int   m_id, m_count;
    double      m_price;
    std::string m_name;
public:
    Material()
        :m_id(0), m_count(0), m_price(0.00), m_name("unknown")
        {}
    Material(const int& id)
        :m_id(id)
        {}
    Material(const int& id, const int& count,
        const double& price, const std::string& name)
        :m_id(id), m_count(count), m_price(price), m_name(name)
        {}
    Material(Material&& obj)
        :m_id(obj.m_id), m_count(obj.m_count),
         m_price(obj.m_price), m_name(obj.m_name)
        {}
    //const std::string& getName()const { return m_name;    }
    Material& operator= (Material& other) = default;
    friend bool operator== (const Material& obj1, const Material& obj2);
};
bool operator== (const Material& obj1, const Material& obj2)
{
    return obj1.m_name == obj2.m_name;
}


int main()
{
    /*Material *Obj = new Material();     Obj = nullptr;
    if(Obj == nullptr)     std::cout << "Equal" << std::endl;*/

    Material Obj1(1,1,2.1,"Jelin"),Obj2(1,1,2.1,"Jeln");
    if(Obj1==Obj2)
        std::cout << "Equal" << std::endl;
    else
        std::cout << "Not Equal" << std::endl;

    return 0;
}
