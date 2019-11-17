#include <iostream>
#include <vector>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <cassert>

template<typename T> class GenericVector final
{
private:
	std::vector<int> _storage;

public:
	template<typename... Args>
	constexpr void emplace_row(Args&& ... args)
	{
		static_assert((std::is_constructible_v<T, Args&&> && ...));
		//assert(_storage.size() <= this->_row); // check col exceeds the size
		(_storage.emplace_back(std::forward<Args>(args)), ...);
	}
};

int main()
{
	GenericVector<int> mat{};
	mat.emplace_row(1, 2);
	mat.emplace_row(3, 4);
	return 0;
}
