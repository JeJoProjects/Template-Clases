/******************************************************************************
 * Memory manager for Signal object. Keeps memory blocks to store
*  Connection<ReType(Args...)> objects. Provides memory allocations / deallocations.
 *
 * @Authur :  JeJo
 * @Date   :  June - 2021
 * @license: free to use and distribute(no further support as well)
 *
 * @todo: explanation and limitations of the class
 *****************************************************************************/

#ifndef JEJO_STORAGE_T_HPP
#define JEJO_STORAGE_T_HPP

 // C++ headers
#include <utility>      // std::exchange


// own JeJo-lib headers
//#include "SlotT.hpp"

namespace JeJo {

	// TEMPLATE CLASS Storage
	template<typename ReType, typename... Args> class Storage;

	template<typename ReType, typename... Args> class Storage<ReType(Args...)> final
	{
	private:
		Connection<ReType(Args...)>* mBlockPtr;
		Connection<ReType(Args...)>* mStorePtr;
		size_type mCapacity;

	private:
		// Initialize requested memory block
		void initMemoryBlock(Connection<ReType(Args...)>* address) noexcept
		{
			this->mStorePtr = address;
			Connection<ReType(Args...)>** current = reinterpret_cast<Connection<ReType(Args...)>**>(address);

			for (size_type index = 1u; index < mCapacity; ++index)
			{
				(*current) = ++address;
				current = reinterpret_cast<Connection<ReType(Args...)>**>(address);
			}

			(*current) = nullptr;
		}

		// Expand memory by allocating new memory block. This May throw exception if memory allocation fails
		void expandMemory()
		{
			Connection<ReType(Args...)>* newMemoryBlock = reinterpret_cast<Connection<ReType(Args...)>*>
				(NEW_MEMORY(
					sizeof(Connection<ReType(Args...)>) * mCapacity + 	sizeof(Connection<ReType(Args...)>*)
				)
			);

			Connection<ReType(Args...)>** nextMemoryBlock = reinterpret_cast<Connection<ReType(Args...)>**>
				(mBlockPtr + mCapacity);

			while (*nextMemoryBlock)
			{
				nextMemoryBlock = reinterpret_cast<Connection<ReType(Args...)>**>((*nextMemoryBlock) + mCapacity);
			}

			(*nextMemoryBlock) = newMemoryBlock;

			// initialize the memory block
			initMemoryBlock(newMemoryBlock);
			(*reinterpret_cast<Connection<ReType(Args...)>**>(newMemoryBlock + mCapacity)) = nullptr;
		}

		// Free all allocated memory blocks
		void clearMemory() noexcept
		{
			if (mBlockPtr)
			{
				Connection<ReType(Args...)>* memoryToDelete = (*reinterpret_cast<Connection<ReType(Args...)>**>(mBlockPtr + mCapacity));

				while (memoryToDelete) // while the memoryToDelete is valid
				{
					Connection<ReType(Args...)>* block = (*reinterpret_cast<Connection<ReType(Args...)>**>(memoryToDelete + mCapacity));
					DELETE_MEMORY(memoryToDelete);
					memoryToDelete = block;
				}

				DELETE_MEMORY(mBlockPtr);
			}
		}

	public:
		// Construct Storage. It may throw exception if memory allocation fails
		Storage(JeJo::size_type capacity)
			: mBlockPtr{ nullptr }
			, mStorePtr{ nullptr }
			, mCapacity{ capacity >= 1u ? capacity : 1u }
		{
			mBlockPtr = reinterpret_cast<Connection<ReType(Args...)>*>(
				NEW_MEMORY(
					mCapacity * sizeof(Connection<ReType(Args...)>) + sizeof(Connection<ReType(Args...)>*)
				)
			);

			// initialize the memory block
			initMemoryBlock(mBlockPtr);
			(*reinterpret_cast<Connection<ReType(Args...)>**>(mBlockPtr + mCapacity)) = nullptr;
		}

		// Copy-construct Storage
		Storage(const Storage& other) noexcept = delete;

		// Copy-assignment Storage
		Storage& operator=(const Storage& other) noexcept = delete;

		// Move-construct Storage
		Storage(Storage&& other) noexcept
			: mBlockPtr{ std::exchange(other.mBlockPtr, nullptr) }
			, mStorePtr{ std::exchange(other.mStorePtr, nullptr) }
			, mCapacity{ std::exchange(other.mCapacity, 0u) }
		{}

		// Move-assignment Storage
		Storage& operator=(Storage&& other) noexcept
		{
			clear();
			if (this != &other)
			{
				mBlockPtr = std::exchange(other.mBlockPtr, nullptr);
				mStorePtr = std::exchange(other.mStorePtr, nullptr);
				mCapacity = std::exchange(other.mCapacity, 0u);
			}

			return *this;
		}

		// Destroy Storage
		~Storage() noexcept
		{
			clearMemory();
		}

		// Allocate memory from Storage; It may throw exception if memory allocation fails
		Connection<ReType(Args...)>* allocate()
		{
			if (!mStorePtr)
			{
				expandMemory(); // May throw
			}

			Connection<ReType(Args...)>* current = mStorePtr;
			mStorePtr = (*reinterpret_cast<Connection<ReType(Args...)>**>(mStorePtr));
			return current;
		}

		// Deallocate previously allocated memory
		void deallocate(Connection<ReType(Args...)>* address) noexcept
		{
			(*reinterpret_cast<Connection<ReType(Args...)>**>(address)) = mStorePtr;
			mStorePtr = address;
		}
	};

}

#endif // JEJO_STORAGE_T_HPP

/*****************************************************************************/