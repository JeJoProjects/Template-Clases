// C++ headers
#include <functional> // std::equal_to<>, std::greater<>, etc.
#include <iterator>   // std::next
#include <cstddef>    // std::size_t
#include <numeric>    // std::inner_product

/* count_adjacent_if: counts all the adjacent pairs of elements in a container*
 * which satisfies the binary predicate passed.
 *
 * @tparam beginIter: forward iterator from which the algorithm should start.
 * @tparam endIter  : forward iterator until which the algorithm should check.
 * @tparam pred     : binary predicate\n
						....std::equal_to<>() by default
 * @return          : count of adjacent pairs which satisfy the condition.
 *
 *  *the container should be such that the begin and end iterators is defined.
 *
 * @todo            : may be it can be extended like lib implementation
 */

template <typename FwdIter, typename BinaryPred = std::equal_to<>> // std::equal_to by default
inline constexpr std::size_t count_adjacent_if(
	FwdIter beginIter, const FwdIter endIter, BinaryPred pred = {})
{
	if (beginIter == endIter) return 0;
	std::size_t count = 0;
	for (FwdIter nextIter = beginIter; ++nextIter != endIter; beginIter = nextIter)
	{
		if (pred(*beginIter, *nextIter))
		{
			++count;
		}
	}
	return count;
}


/* count_adjacent_if2: counts all the adjacent pairs of elements in a container*
 * which satisfies the binary predicate passed.
 *
 * @tparam beginIter: forward iterator from which the algorithm should start.
 * @tparam endIter  : forward iterator until which the algorithm should check.
 * @tparam pred     : binary predicate\n
						....std::equal_to<>() by default
 * @return          : count of adjacent pairs which satisfy the condition.
 *
 *  @todo           : it has a severe bug which needed to be fixed.
 *  *the container should be such that the begin and end iterators is defined.
 */
template <typename FwdIter, typename BinaryPred = std::equal_to<>>
inline constexpr std::size_t count_adjacent_if2(
	FwdIter beginIter, const FwdIter endIter, BinaryPred pred = {})
{
	const auto n = std::distance(beginIter, endIter);
	if (n < 2) return std::size_t{ 0 };
	return std::inner_product(
		beginIter, endIter, std::next(beginIter, n - 1), std::size_t{ 0 }, std::plus<>{}, pred);
}