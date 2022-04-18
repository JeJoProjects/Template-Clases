#include <algorithm>
#include <iostream>
#include <vector>
#include <iterator>
#include <type_traits>

template<class T>
struct has_member final 
{
    template<class> static auto SortTest(...)->std::false_type;

    template<class TT>
    static auto SortTest(int) ->
        decltype(std::declval<TT&>().sort(), std::true_type());

    template<class> static auto RangeTest(...)->std::false_type;

    template<class TT>
    static auto RangeTest(int) ->
        decltype(std::begin(std::declval<TT&>()),
            std::end(std::declval<TT&>()),
            std::true_type());

    static constexpr bool sort_value = decltype(SortTest<T>(0))::value;

    static constexpr bool range_value = decltype(RangeTest<T>(0))::value;
};
template<class T> inline static constexpr bool has_sort_v = has_member<T>::sort_value;
template<class T> inline static constexpr bool has_range_v = has_member<T>::range_value;



template<typename T>
void fast_sort(T& x)
{
    if constexpr (has_sort_v<T>) {
        std::cout << "has_sort\n";
        x.sort();
    }
    else if constexpr (has_range_v<T>) {
        std::cout << "has_range\n";
        std::sort(std::begin(x), std::end(x));
    }
    else {
        std::cout << "bohoo\n";
    }
}

class A {};
class B { public: void sort() {} };

int main()
{
    std::vector<int> V{ 1,9,5,3 };
    A a;
    B b;
    fast_sort(V);
    fast_sort(a);
    fast_sort(b);
}
