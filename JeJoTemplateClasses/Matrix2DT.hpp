#include <iostream>
#include <vector>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <cassert>

template<typename T> class Matrix2D
{
private:
	const std::size_t _row;
	const std::size_t _col;
	std::vector<int> _storage;

public:
	constexpr explicit Matrix2D(
		const std::size_t row, const std::size_t col)
		: _row{ row }
		, _col{ col }
	{
		_storage.reserve(_row * _col);
	}

	template<typename... Args>
	constexpr void emplace_row(Args&& ... args)
	{
		static_assert((std::is_constructible_v<T, Args&&> && ...));
		assert(sizeof...(Args) <= this->_row);
		//assert(_storage.size() <= this->_row); // check col exceeds the size
		(_storage.emplace_back(std::forward<Args>(args)), ...);
	}
};

int main()
{
	Matrix2D<int> mat{ 2, 2 };
	mat.emplace_row(1, 2);
	mat.emplace_row(3, 4);
	return 0;
}
