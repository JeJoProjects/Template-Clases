/******************************************************************************
 * Implementation of some algorithms which accepts binary predicates.
 * Some of them can be found in standard c++ <algorithum> headers,
 * but most of them are restricted for binary predicates.
 *
 * @Authur :  JeJo
 * @Date   :  June - 2018
 * @license: free to use and distribute(no further support as well)
 *
 * Individual explanations are provided, prior to each algorithm definitions.
 *****************************************************************************/

#ifndef JEJO_ALGORITHMS_T_H
#define JEJO_ALGORITHMS_T_H

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// C++ headers
#include <functional> // std::equal_to<>, std::greater<>, etc.
#include <iterator>   // std::next
#include <cstddef>    // std::size_t
#include <numeric>    // std::inner_product
#include <map>        // std::map<>

JEJO_BEGIN

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

template <typename FwdIter, typename BinaryPred = std::equal_to<>>
inline constexpr std::size_t count_adjacent_if(
	FwdIter beginIter, const FwdIter endIter, const BinaryPred pred = {}) noexcept
{
	if (beginIter == endIter) return 0;
	std::size_t count = 0;
	for (FwdIter nextIter = beginIter;
		++nextIter != endIter;
		beginIter = nextIter)
	{
		if (pred(*beginIter, *nextIter))	++count;
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
	FwdIter beginIter, const FwdIter endIter, BinaryPred pred = {}) noexcept
{
	const auto n = std::distance(beginIter, endIter);
	if (n < 2) return std::size_t{ 0 };
	return std::inner_product(
		beginIter, endIter,
		std::next(beginIter, n - 1), std::size_t{ 0 }, std::plus<>{}, pred);
}

/* has_duplicates: using std::map, counts the occurrence of the particular
 * elements in the container and...\n
 *
 * @return  : true.... duplicates exists
 *            false... if not
 */

template <typename FwdIter>
inline constexpr bool has_duplicates(FwdIter beginIter, const FwdIter endIter)
{
	if (beginIter == endIter) return false; // edge case
	using Type = typename std::decay<decltype(*beginIter)>::type;
	std::map<Type, std::size_t> countMap;
	for (; beginIter != endIter; ++beginIter)
	{
		countMap[*beginIter]++;
		if (countMap[*beginIter] >= 2) return true;
	}
	return false;
}

JEJO_END

#endif // JEJO_ALGORITHMS_T_H

/*****************************************************************************/
