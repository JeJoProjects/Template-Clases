#include <iostream>
#include <type_traits>
#include <vector>
#include <utility>
#include <algorithm>

template<typename T> class GenericVectorT;

// traits for checking the Type is `std::vector`
template<typename> struct is_GenericVector final : std::false_type {};
template<typename T>
struct is_GenericVector<GenericVectorT<T>> final : std::true_type {};


template<typename T> struct is_SimpleGenericVector final 
{
	static constexpr bool value = is_GenericVector<T>::value == false && std::is_floating_point<T>::value == true;
};



template<typename Type> class GenericVectorT final
{
private:
	std::vector<Type> mStorage;

public:
	template<typename T = Type>
	auto updateMinMax(std::pair<T, T>& minMaxCompare) noexcept
		-> std::enable_if_t<is_SimpleGenericVector<T>::value>
	{
		for (const T element : mStorage)
		{
			minMaxCompare.first = std::min(minMaxCompare.first, element);
			minMaxCompare.second = std::min(minMaxCompare.second, element);
		}
	}
#if 1
	template<typename T = Type>
	auto updateMinMax(std::pair<T, T>& minMaxCompare) noexcept
		-> std::enable_if_t<!is_SimpleGenericVector<T>::value>
	{
		std::cout << "Here...\n";
	}
#endif
};
