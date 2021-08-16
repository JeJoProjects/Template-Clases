#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <array>
#include <numeric>
#include <type_traits> // std::is_floating_point_v, std::is_integral_v, std::is_same_v
                       // std::remove_const_t, std::remove_reference_t

// traits for finding the innermost template type
// https://stackoverflow.com/questions/25187323/
template<typename Type> struct inner_most final { using type = Type; };

template<template<typename...> class ClassType, typename Type, typename... Rest>
struct inner_most<ClassType<Type, Rest...>> final
{
   using type = typename inner_most<Type>::type;
};

template <typename T, std::size_t N>
struct inner_most<std::array<T, N>> final
{
   using type = typename inner_most<T>::type;
};

template<typename T> using innermost_t = typename inner_most<T>::type;


// allowed types for finding the min and max!
template<typename Type> 
inline static constexpr bool isAllowedType
	= std::is_floating_point_v<Type> || std::is_integral_v<Type>;
// || std::is_same_v<A, Type>; other types....


template<typename T>
std::ostream& operator<<(std::ostream& out, const std::pair<T, T>& pair) noexcept
{
   return out << pair.first << " " << pair.second << "\n";
}

// Generic class template implementation
template<typename Type>
class GenericVectorT final : public std::vector<Type>
{
   // convenience types!
   using InnerType = innermost_t<Type>;
   using MinMaxPair = std::pair<InnerType, InnerType>;

   // default min and max value for comparing the elements in the vector
   inline static constexpr MinMaxPair DEFAULT_VALUE {
      std::numeric_limits<InnerType>::max(),
      std::numeric_limits<InnerType>::min()
   };

   // helper functions are implemented as part of the internal struct
   struct Helper final
   {
      template<typename Vector>
      static constexpr MinMaxPair getmMinMaxElements(
         const Vector& vec, const MinMaxPair& minMaxPair) noexcept
      {
         MinMaxPair result{ minMaxPair};
         // value type of the passed `GenericVectorT<Type>`
         using ValueType = std::remove_const_t<
            std::remove_reference_t<typename Vector::value_type>
         >;
         // if it is allowed type, to update the min and max values!
         if constexpr (isAllowedType<ValueType>)
         {
            const auto [min, max] = std::minmax_element(std::cbegin(vec), std::cend(vec));
            result.first = min != std::cend(vec) ? std::min(result.first, *min) : result.first;
            result.second = max != std::cend(vec) ? std::max(result.second, *max): result.second;
            return result; // return the result!
         }
         else
         {
            for (const ValueType& innerVec : vec)
            {
               const auto& [min, max] = getmMinMaxElements(innerVec, minMaxPair);
               result.first = std::min(result.first, min);
               result.second = std::max(result.second, max);
            }
            return result; // return the result!
         }
      }
   };

public:
	using std::vector<Type>::vector;

   constexpr MinMaxPair updateMinMax(const MinMaxPair& minMaxPair = DEFAULT_VALUE) const noexcept
	{
      return Helper::getmMinMaxElements(*this, minMaxPair);
	}
};