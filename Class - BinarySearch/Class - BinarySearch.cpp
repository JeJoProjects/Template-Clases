#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <exception>
#include <initializer_list>

template<typename Type>
class BinarySearch
{
   std::vector<Type> m_vec;
   Type val_index = std::numeric_limits<Type>::min();
public:
   // initializer_list construcor
   BinarySearch(const std::initializer_list<Type>& a): m_vec(a) {}

   const Type& getIndex()const   {  return val_index; }

   bool check_this(Type Start, Type End, const Type& val)
   {
      try
      {
         // if not sorted, do not try to find the value
         if(!std::is_sorted(m_vec.begin(), m_vec.end())) throw std::runtime_error( "Array is not sorted!" );

         if(Start <= End)
         {
            const int mid_index = Start + ( (End - Start) / 2 );
            // Find the middle element -> Ckeck ->  split
            if(m_vec[mid_index] == val)      { val_index = mid_index; return true;  }
            else if(m_vec[mid_index] < val)  return check_this(mid_index + 1, End, val);
            else                             return check_this(Start, mid_index - 1, val);
         }

      }
      catch(std::runtime_error& e)
      {
         std::cout << e.what() << std::endl;
      }
      val_index = std::numeric_limits<Type>::min();
      return false;
   }

};


int main()
{
   BinarySearch< int > Arr = {1, 2,  3, 4, 5, 8};

   for(int i = 1; i <= 10 ; ++i)
     Arr.check_this(0, 5, i ) ?
      std::cout << i <<" found at index: " << Arr.getIndex() << std::endl :
      std::cout << i << " Not" << std::endl;

   return 0;
}
