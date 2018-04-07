#include <numeric>
#include <functional>
#include <iterator>

#include <vector>
#include <iostream>

template <typename ForwardIterator, typename BinaryPredicate>
auto count_pairs_if(ForwardIterator first, ForwardIterator last,
                    BinaryPredicate pred)
{
    const auto n = std::distance(first, last);
    if (n < 2) return std::size_t {0};
    return std::inner_product(first, std::next(first, n - 1), std::next(first),
                              std::size_t {0}, std::plus<> {}, pred);
}

template <typename Range, typename BinaryPredicate>
auto count_pairs_if(const Range& values, BinaryPredicate pred)
{
    return count_pairs_if(std::cbegin(values), std::cend(values), pred);
}

int main()
{
    constexpr int Number {3};
    std::vector<int> vec {1,1,2,4,5,6};

    auto count = count_pairs_if(vec, [=] (auto lhs, auto rhs) { return lhs + rhs == Number; });

    std::cout << count << std::endl;
    return 0;
}
