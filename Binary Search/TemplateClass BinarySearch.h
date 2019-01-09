#include <vector>            // std::vector<>
#include <limits>            // std::numeric_limits
#include <algorithm>         // std::is_sorted
#include <exception>         // std::runtime_error
#include <initializer_list>  // std::initializer_list<>
#include <type_traits>       // std::is_integral_v, std::is_floating_point_v


// variable template for convenice use of std::numeric_limits
template<typename Type> inline constexpr Type typeMinimum = std::numeric_limits<Type>::min();

template<typename Type> class BinarySearch
{
private:
	// internal storage of array
	std::vector<Type> _vec;
	// index of the searching element (all other non-integer cases)
	std::size_t _index = 0;

private:
	// resetter to set the defualt(min) value
	void make_default_index() noexcept
	{
		if constexpr (std::is_integral_v<Type> || std::is_floating_point_v<Type>)
		{
			_index = static_cast<std::size_t>(typeMinimum<Type>);
			return;
		}
	}

	// helper function to search specific element in the array
	// and returns: true... if found!
	// Indition to that: save the position of the element in the array
	bool search(std::size_t Start, std::size_t End, const Type& val)
	{
		if (End == 0) return false;
		try
		{
			// if not sorted, do not try to find the value
			if (!std::is_sorted(_vec.begin(), _vec.end())) throw std::runtime_error("Array is not sorted!\n");

			if (Start <= End)
			{
				const std::size_t mid_index = Start + ((End - Start) / 2);
				// Find the middle element -> Ckeck ->  split
				if (_vec[mid_index] == val) { _index = mid_index; return true; }
				else if (_vec[mid_index] < val)  return search(mid_index + 1, End, val);
				else                             return search(Start, mid_index - 1, val);
			}
		}
		catch (const std::runtime_error& e) { std::cout << e.what() << '\n'; 	}
		this->make_default_index();
		return false;
	}

public:
	// initializer_list construcor
	explicit BinarySearch(const std::initializer_list<Type> a) : _vec(std::move(a)) { make_default_index(); }

	// getter for index
	std::size_t getIndex() const  noexcept { return _index; }

	// member function to search specific elements in the array
	bool search(const Type& val) { return this->search(0, _vec.empty() ? 0 : _vec.size() - 1, val); }
};


