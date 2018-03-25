#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>

/**************** Item: defined as a pure abstract class ************************ **/
class Item
{
protected:
    int m_code;
    int m_price;
    int m_quantity;
public:
    Item() = default;

    void setCode(const int& code)   {   m_code = code;  }
    void setPrice(const int& price)   {   m_price = price;  }
    void setQuantity(const int& quantity)   {   m_quantity = quantity;  }

    /*const int& getCode()const { return m_code;  }
    const int& getPrice()const { return m_price;  }
    const int& getQuantity()const { return m_quantity;  }*/

    virtual int calculateStockValue() = 0;
    virtual ~Item(){ };
};

/** ItemTable : inherits from Item, to store code - price - quantity to hash table **/
class ItemTable: public Item
{
private:
    std::unordered_map<int, std::pair<int,int>> m_itemTable;
public:
    ItemTable() = default;
    //(ii) to display the data members according to the item code
    void displayItem(const int& code)
    {
        if(m_itemTable.find(code) != m_itemTable.cend())
        {
            auto it = m_itemTable.find(code);
            std::cout << "Item code = " << it->first;
            std::cout << "\t price = " << it->second.first;
            std::cout << "\t quantity = " << it->second.second << std::endl;
        }
        else
            std::cout << "Invalid Item code!" << std::endl;
    }
    // (iii) appending Item to the list
    void appendItem(const int& code, const  int& price, const int& quantity)
    {
        //(i) input the data members
        setCode(code);
        setPrice(price);
        setQuantity(quantity);
        m_itemTable.emplace(m_code, std::make_pair(m_price, m_quantity));
    }
    // (iv) delete the Item from the list
    void deleteItem(const int& code)
    {
        if(m_itemTable.find(code) != m_itemTable.cend())
        {
            std::cout << "The item with code: " << code
                    << " has been deleted successfully!" <<std::endl;
            m_itemTable.erase(m_itemTable.find(code));
        }
        else
            std::cout << "Invalid Item code!" << std::endl;
    }
    // (v) total value of the stock
    virtual int calculateStockValue() override
    {
        int m_totalStock = 0;
        for(const auto &it:m_itemTable)
            m_totalStock += (it.second.first * it.second.second);

        return m_totalStock;
    }
    virtual ~ItemTable(){ };
};
/********************* main ******************************/
int main()
{
    int number;
    int code;
    std::unique_ptr<ItemTable> obj = std::make_unique<ItemTable>();

    std::cout << "Enter total number of Items : ";  std::cin >> number;

    for(int i = 0; i<number; ++i)
    {
        int code, price, quantity;
        std::cout << i+1 << ": Enter the Item code = "; std::cin >> code ;
        std::cout << i+1 << ": Enter the Item price = "; std::cin >> price ;
        std::cout << i+1 << ": Enter the Item quantity = "; std::cin >> quantity ;
        obj->appendItem(code, price, quantity);
    }

    std::cout << "Total Value of stock = " << obj->calculateStockValue() << std::endl;

    std::cout << "Enter code of the Item to be displayed: "; std::cin >> code;
    obj->displayItem(code);

    std::cout << "Enter code to delete the Item : ";         std::cin >> code;
    obj->deleteItem(code);

    std::cout << "Total Value of stock = " << obj->calculateStockValue() << std::endl;

    return 0;
}
