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
#include <cstddef>		// std::size_t, std::nullptr_t
#include <utility>		// std::move(), std::forward<>()
#include <algorithm>	// std::copy(), std::find_if()
#include <array>        // std::array<>, std::cbegin(), ::std::cend()
#include <functional>   // std::invoke()
#include <new>			// new()
#include <atomic>		// std::atomic<>, std::atomic_flag
#include <memory>		// std::shared_ptr<>, std::weak_ptr<>
#include <thread>		// std::this_thread


// own JeJo-lib headers
#include "SlotT.hpp"

namespace JeJo {

	// TEMPLATE CLASS Slot
	template<typename ReType, typename... Args> class Storage;

	template<typename ReType, typename... Args> class Storage<ReType(Args...)> final
	{
	private:

		Connection<ReType(Args...)>* mp_block;
		Connection<ReType(Args...)>* mp_store;
		size_type		m_capacity;

	private:
		// Initialize requested memory block
		void init(Connection<ReType(Args...)>* address) noexcept
		{
			mp_store = address;
			Connection<ReType(Args...)>** current = reinterpret_cast<Connection<ReType(Args...)>**>(address);

			for (size_type index = 1; index < m_capacity; ++index)
			{
				(*current) = ++address;
				current = reinterpret_cast<Connection<ReType(Args...)>**>(address);
			}

			(*current) = nullptr;
		}

		// Expand memory by allocating new memory block
		// May throw exception if memory allocation fails
		void expand()
		{
			Connection<ReType(Args...)>* new_block = reinterpret_cast<Connection<ReType(Args...)>*>
				(NEW_MEMORY(sizeof(Connection<ReType(Args...)>) * m_capacity +
					sizeof(Connection<ReType(Args...)>*)));
			Connection<ReType(Args...)>** next_block = reinterpret_cast<Connection<ReType(Args...)>**>
				(mp_block + m_capacity);

			while (*next_block)
			{
				next_block = reinterpret_cast<Connection<ReType(Args...)>**>
					((*next_block) + m_capacity);
			}

			(*next_block) = new_block;
			init(new_block);
			(*reinterpret_cast<Connection<ReType(Args...)>**>
				(new_block + m_capacity)) = nullptr;
		}

		// Free all allocated memory blocks
		void clear() noexcept
		{
			if (mp_block)
			{
				Connection<ReType(Args...)>* to_delete = (*reinterpret_cast<Connection<ReType(Args...)>**>
					(mp_block + m_capacity));

				while (to_delete)
				{
					Connection<ReType(Args...)>* block = (*reinterpret_cast<Connection<ReType(Args...)>**>
						(to_delete + m_capacity));
					DELETE_MEMORY(to_delete);
					to_delete = block;
				}

				DELETE_MEMORY(mp_block);
			}
		}

	public:

		// Construct Storage
		// May throw exception if memory allocation fails
		Storage(JeJo::size_type capacity)
			: mp_block{ nullptr }
			, mp_store{ nullptr }
			, m_capacity{ capacity >= 1u ? capacity : 1u }
		{
			mp_block = reinterpret_cast<Connection<ReType(Args...)>*>(
				NEW_MEMORY(
					m_capacity * sizeof(Connection<ReType(Args...)>)
					+ sizeof(Connection<ReType(Args...)>*)
				)
				);
			init(mp_block);

			(*reinterpret_cast<Connection<ReType(Args...)>**>
				(mp_block + m_capacity)) = nullptr;
		}

		// Move-construct Storage
		Storage(Storage&& other) noexcept
			: mp_block(other.mp_block),
			mp_store(other.mp_store),
			m_capacity(other.m_capacity)
		{
			other.mp_block = nullptr;
			other.mp_store = nullptr;
			other.m_capacity = 0;
		}

		// Destroy Storage
		~Storage() noexcept
		{
			clear();
		}

		// Move-assign Storage
		Storage& operator=(Storage&& other) noexcept
		{
			clear();

			mp_block = other.mp_block;
			mp_store = other.mp_store;
			m_capacity = other.m_capacity;

			other.mp_block = nullptr;
			other.mp_store = nullptr;
			other.m_capacity = 0;

			return *this;
		}

		// Allocate memory from Storage
		// May throw exception if memory allocation fails
		Connection<ReType(Args...)>* allocate()
		{
			if (!mp_store)
			{
				expand(); // May throw
			}

			Connection<ReType(Args...)>* current = mp_store;
			mp_store = (*reinterpret_cast<Connection<ReType(Args...)>**>(mp_store));
			return current;
		}

		// Deallocate previously allocated memory
		void deallocate(Connection<ReType(Args...)>* address) noexcept
		{
			(*reinterpret_cast<Connection<ReType(Args...)>**>(address)) = mp_store;
			mp_store = address;
		}
	};

}

#endif // JEJO_STORAGE_T_HPP

/*****************************************************************************/