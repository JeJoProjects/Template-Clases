#include <iostream>
#include <string>

/** Class - 1:  ******************************************************/
class SectionClass
{
public:
    virtual const std::string printClassName()const = 0;
    virtual const std::string& getName()const = 0;
    virtual ~SectionClass() {}
};

/** Class - 2:  ******************************************************/
class Company : public SectionClass
{
private:
    std::string m_companyName;
public:
    Company(): m_companyName("Vector Informatik") { std::cout<<"default"<<std::endl; }
    ~Company(){ std::cout<< this->m_companyName <<" Destroyed"<<std::endl<<std::endl; }
    const std::string printClassName()const override { return "Company :"; }
    const std::string& getName()const { return m_companyName; }
};

/** Class - 3:  ******************************************************/
class Position: public SectionClass
{
private:
    std::string m_Position;
public:
    Position(): m_Position("Name of the Position") { std::cout<<"default"<<std::endl; }
    ~Position() { std::cout<< this->m_Position <<" Destroyed"<<std::endl<<std::endl; }
    const std::string printClassName()const override { return "Position :"; }
    const std::string& getName()const override{ return m_Position; }
};

/** Class - 4:  ******************************************************/
class Person : public SectionClass
{
private:
    int age;
    std::string name;

public:
    Person()                              : age(0), name("Unknown1")
    {   std::cout<<"default"<<std::endl;  }

    Person(const int &a, const std::string &n) : age(a), name(n)
    {   std::cout<<"assgin1"<<std::endl;  }

    Person(Person &obj)                   : age(obj.age), name(obj.name)
    {   std::cout<<"Copy"<<std::endl;  }

    ~Person() { std::cout<< this->name <<" Destroyed"<<std::endl<<std::endl; }

    void inputName(const std::string &n) { name = n; }


    const std::string& getName()const override {    return name;    }
    const std::string printClassName()const override { return "Person :"; }
};

/** ******* End of All classes *******************************************/
std::ostream& operator<<(std::ostream &out, const SectionClass &obj)
{
    return out << obj.getName();;
}

void printClassName(SectionClass* obj)
{
    std::cout<<obj->printClassName()<<std::endl;
}

int main()
{
    Person *P2 = new Person(); //0 Unknown1

    P2->inputName("Jelin John");
    std::cout<<*P2<<std::endl;
    printClassName(P2);
    delete P2;      P2 = nullptr;

    Company *c = new Company();
    std::cout<<*c<<std::endl;
    printClassName(c);
    delete c; c = nullptr;

    Position *Pos = new Position();
    std::cout<<*Pos<<std::endl;
    printClassName(Pos);
    delete Pos; Pos = nullptr;

    std::cout<<std::endl<<std::endl<<sizeof(P2)<<std::endl;
    std::cout<<sizeof(c)<<std::endl;
    std::cout<<sizeof(Pos)<<std::endl;

    return 0;
}
