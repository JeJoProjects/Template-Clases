#include <iostream>
#include <string>
#include <unordered_map>

/** Class which has been asked in the question. In short Iteam class will act like
    a struct for iteamList class, since I have declared iteamList class as a friend
    of Iteam class **/
class iteamList; // forward delaration of iteamList class
class Iteam
{
private:
    int m_code;
    int m_price;
    int m_quantity;
public:
    Iteam()                                     // Defualt
        :m_code(0), m_price(0), m_quantity(0)
        {}
    Iteam(const int& code, const  int& price, const int& quntity)// Parameterized
        :m_code(code), m_price(price), m_quantity(quntity)
        {}
    Iteam(const Iteam& rhs)                     // Copy
        :m_code(rhs.m_code), m_price(rhs.m_price), m_quantity(rhs.m_quantity)
        {}
    Iteam(Iteam&& rhs)                          // Move
        :m_code(rhs.m_code), m_price(rhs.m_price), m_quantity(rhs.m_quantity)
        {}
    ~Iteam()     {}

    //(i) input the data members
    inline void inputData()
    {
        std::cout<<"Iteam code :"; std::cin>>m_code;
        std::cout<<"Iteam price :"; std::cin>>m_price;
        std::cout<<"Iteam quantity :"; std::cin>>m_quantity;
    }
    // life would be easy after making this statement :) !
    friend iteamList;
    //(ii) to display the data members
    friend std::ostream& operator<<(std::ostream& output, const Iteam& obj);
};
std::ostream& operator<<(std::ostream& output, const Iteam& obj)
{
    return output<<"Iteam code = "<<obj.m_code
                <<"\t price = "<<obj.m_price
                <<"\t quantity = "<<obj.m_quantity<<"\n";
}
/******************************************************************************/
/** Implementation of the main class which is independent of the above one.
    It can access all members of above class if it has instantiated an object of it.
    I have used STL std::unordered_map to store the Items which will be easier to
    look up(with an O(1)) at the time of queries of deletion.**/

class iteamList
{
private:
    int m_totalStockValue = 0;
    std::unordered_map<int, Iteam> m_iteamMap;
private:
    // (iii) appending iteam to the list
    inline void AppendIteams(const int& userInput_n)
    {
        for(auto i=0; i<userInput_n; ++i)
        {
            Iteam obj;
            obj.inputData();
            std::cout << obj << std::endl;
            m_iteamMap.emplace(obj.m_code, obj);
        }
    }
public:
    iteamList() = default;

    iteamList(const int& userInput_n)
    {
        m_iteamMap.reserve(userInput_n);
        AppendIteams(userInput_n);
    }
    ~iteamList(){}

    // (iv) delete the iteam from the list
    void deleteIteam(const int& code)
    {
        if(m_iteamMap.find(code) != m_iteamMap.cend())
        {
            std::cout << m_iteamMap[code] << " has been deleted!\n";
            m_iteamMap.erase(m_iteamMap.find(code));
        }
        else
            std::cout <<"Invalid Iteam code!" << std::endl;
    }
    // (v) total value of the stock
    void claculateStockValue()
    {
        m_totalStockValue = 0;
        for(const auto &it:m_iteamMap)
            m_totalStockValue += (it.second.m_price * it.second.m_quantity);

        std::cout <<m_totalStockValue<< std::endl;
    }
};
/**********************************************************************************/
int main()
{
    int number;
    std::cout <<"Enter total number of iteams : ";
    std::cin>>number;

    iteamList obj(number);

    std::cout <<"Total Value of stock = ";
    obj.claculateStockValue();

    int code;
    std::cout <<"Enter code to delete the iteam : ";
    std::cin>>code;
    obj.deleteIteam(code);

    std::cout <<"Total Value of stock = ";
    obj.claculateStockValue();

    return 0;
}
