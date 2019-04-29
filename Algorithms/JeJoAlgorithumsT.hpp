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

#ifndef JEJO_ALGORITHMS_T_HPP
#define JEJO_ALGORITHMS_T_HPP

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// C++ headers
#include <functional> // std::equal_to<>, std::greater<>, etc.
#include <iterator>   // std::next
#include <cstddef>    // std::size_t
#include <numeric>    // std::inner_product
#include <algorithm>  // std::min_element
#include <map>        // std::map<>
#include <memory>     // smart pointers
#include <utility>    // std::declval
#include <functional> // std::cref
#include <array>

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

template <typename Iterator, typename BinaryPred = std::equal_to<>>
constexpr std::size_t count_adjacent_if(
	      Iterator beginIter,
	const Iterator endIter,
	const BinaryPred pred = {}) noexcept
{
	if (beginIter == endIter) return 0;
	std::size_t count = 0;
	for (Iterator nextIter = beginIter; ++nextIter != endIter;
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
template <typename Iterator, typename BinaryPred = std::equal_to<>>
constexpr std::size_t count_adjacent_if2(
	Iterator beginIter, const Iterator endIter, const BinaryPred pred = {})
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
template <typename Iterator>
inline constexpr bool has_duplicates(Iterator beginIter, const Iterator endIter)
{
	if (beginIter == endIter) return false; // edge case
	using Type = typename std::remove_reference_t<decltype(*beginIter)>;
	std::map<Type, std::size_t> countMap;
	for (; beginIter != endIter; ++beginIter)
	{
		countMap[*beginIter]++;
		if (countMap[*beginIter] >= 2) return true;
	}
	return false;
}

/* min_element_range_of: @todo: description
 *
 * @return  : minimum element in the container
 */
template<typename Container, typename Predicate = std::less<>>
auto min_element_range_of(const Container &container,
	std::size_t startIdx,
	std::size_t endIdx,
	const Predicate pred = {})
	-> std::remove_reference_t<decltype(*(container.begin()))>
	         // or simply: // typename Container::value_type
{
	return *std::min_element(
		std::next(container.begin(), startIdx),
		std::next(container.begin(), ++endIdx), pred);
}

/* To avoid copying the smart pointers, during the initialization of a
 * std::initializer_list without using raw pointers.
 * StackOverflow: https://stackoverflow.com/questions/54868996/54869137
 *
 * Since crefRange(const-ref T...) accepts temporaries, there is a chance of
 * building an array of dangling (const)std::reference_wrapper's,
 * it is essential to delete the possible template instance explicitly.
 */

template <class ...T> auto crefPointers(const T&&...) = delete;

template <class ...T> auto crefPointers(const T&... args)
{
	// type alias for type of the first element in the args
	using FirstType = std::tuple_element_t<0, std::tuple<T...>>;
	// type alias for (const)std::reference_wrapper to the 'FirstType'
	using CRefFirst = decltype(std::cref(std::declval<FirstType&>()));
	// making an array of type = 'CRefFirst'
	return std::array<CRefFirst, sizeof...(T)>{ { std::cref(args)... } };
}

JEJO_END

#endif // JEJO_ALGORITHMS_T_HPP

/*****************************************************************************/
