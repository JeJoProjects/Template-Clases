#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

template<typename T>
class VectorExt : public std::vector<T>
{
public:
    using std::vector<T>::vector;

    const std::string elementString() const
    {
        std::string str;    str.clear();
        for(const auto &it: *this)
        {
            str += std::to_string(it);
            str += " ";
        }
        return str;
    }
    // extended functionality
    friend constexpr std::ostream& operator<<
    (std::ostream& output, const VectorExt<T>& obj)
    {   return output<<obj.elementString(); }
};

int main()
{
    VectorExt<int> obj(10);
    std::cout<<obj<<std::endl;  // works fine as expected

    VectorExt<int64_t> Obj1 = {1,8,2,3,4,7,5,6,9};
    std::cout<<Obj1<<std::endl; // works fine as expected

    std::cout<<Obj1.size()<<std::endl; // works fine as expected

    VectorExt<int> vec = {10,2,5,4,7,8,3};
    std::sort(vec.begin(),vec.end());
    std::cout<<vec<<std::endl;

    return 0;
}

