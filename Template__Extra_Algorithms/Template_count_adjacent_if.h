/* extra::count_adjacent_if: counts all the adjacent pairs of elements in a container* 
 * which satisfies the binary predicate passed.
 *
 * @tparam beginIter: forward iterator from which the algorithum should start.
 * @tparam endIter  : forward iterator until which the algorithum should check.
 * @tparam pred     : binary predicate\n
					    ....std::equal_to<>() by default
 * @return          : count of adjucent pairs which satisfys the condition.
 *
 *  *the container should be such that the begin and end iterators is defined.
 */

// C++ headers
#include <algorithm>
#include <functional> // std::equal_to, std::greater<>, etc.

#define _EXTRA_BEGIN	namespace extra {
#define _EXTRA_END	}

  _EXTRA_BEGIN // namespace extra

// @todo: extend with lib implimentation

template <typename FwdIter, typename BinaryPred = std::equal_to<void>> // std::greater<void>
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

_EXTRA_END // namespace extra

