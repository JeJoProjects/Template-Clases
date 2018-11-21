/* count_adjacent_if
 *
 */

#include <vector>
#include <iostream>
#include <algorithm>

#define _EXTRA_BEGIN	namespace extra {
#define _EXTRA_END	}

  _EXTRA_BEGIN // namespace extra
// @todo: extend with lib implimentation
template <typename FwdIter>
inline constexpr void verify_range(
	const FwdIter &beginIter, const FwdIter &endIter)
{
	_Adl_verify_range(beginIter, endIter);
}


template <typename FwdIter, typename BinaryPred>
inline constexpr std::size_t count_adjacent_if(
	FwdIter beginIter, const FwdIter endIter, const BinaryPred &pred)
{
	verify_range(beginIter, endIter);
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

//
template <typename FwdIter>
inline constexpr std::size_t count_adjacent_if(const FwdIter _First, const FwdIter _Last)
{	// find first matching successor
	return count_adjacent_if(_First, _Last, std::equal_to<>());
}
_EXTRA_END // namespace extra

int main()
{
	std::vector<int> vec{ 1,1,2,4,5,5,6,6 };
	auto BinPred = [](const auto& lhs, const auto& rhs) { return lhs == rhs; };
	std::cout << extra::count_adjacent_if(vec.begin(), vec.end()) << std::endl;
	return 0;
}
