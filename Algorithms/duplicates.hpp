// C++ headers
#include <map>
#include <cstddef>    // std::size_t

/* has_duplicates: using std::map counts the occurrence of the particular elements in
 * in the container and...\n
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
