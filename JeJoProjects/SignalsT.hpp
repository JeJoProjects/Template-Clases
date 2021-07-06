/******************************************************************************
 * Implementation of Signal<>, which provides a template 
 *
 * @Authur :  JeJo
 * @Date   :  June - 2021
 * @license: free to use and distribute(no further support as well)
 *
 * @todo: explanation and limitations of the class
 *****************************************************************************/

#ifndef JEJO_SIGNALS_T_HPP
#define JEJO_SIGNALS_T_HPP

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
#include <string>
#include <iostream>
#include <utility>

#include "SlotT.hpp"
#include "StorageT.hpp"

// own JeJo-lib headers

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }


JEJO_BEGIN


// TEMPLATE CLASS Signal
template<typename ReType, typename... Args>
class Signal;

template<typename ReType, typename... Args>
class Signal<ReType(Args...)> final
{

	// Synchronization primitive that protects shared data from being
	// simultaneously modified by multiple threads. Optimized for speed
	// and occupies very little memory. Meets Lockable requirements.
	class SlimLock;

	// ClassType AutoLock is a SlimLock ownership wrapper that provides a
	// convenient RAII-style mechanism for automatic locking / unlocking.
	class AutoLock;

	// ClassType ReadGuard is a stage counter ownership wrapper. Provides
	// a convenient RAII-style mechanism for automatic increm / decrem.
	class ReadGuard;
	
	using ConnectionType = Connection<ReType(Args...)>;
	using ConnectionPtr = ::std::atomic<Connection<ReType(Args...)>*>;


	class SlimLock final
	{
		using lock_type = ::std::atomic_flag;

	public:

		// Construct SlimLock
		SlimLock() noexcept
		{}

		// Deleted copy-constructor
		SlimLock(const SlimLock&)noexcept = delete;

		// Destroy SlimLock
		~SlimLock() noexcept
		{}

		// Deleted copy-assignment operator
		SlimLock& operator=(const SlimLock&)noexcept = delete;

		// Lock SlimLock. If another thread has already locked this
		// SlimLock, blocks execution until the lock is acquired.
		void lock() noexcept
		{
			while (m_lock.test_and_set(::std::memory_order_acquire))
			{
				::std::this_thread::yield();
			}
		}

		// Unlock SlimLock
		void unlock() noexcept
		{
			m_lock.clear(::std::memory_order_release);
		}

		// Try to lock SlimLock. Returns immediately. On successful
		// lock acquisition returns true, otherwise returns false.
		bool try_lock() noexcept
		{
			return m_lock.test_and_set(::std::memory_order_acquire)
				? false : true;
		}

	private:

		lock_type m_lock = ATOMIC_FLAG_INIT;

	};

	class AutoLock
	{
	public:

		// Construct AutoLock
		explicit AutoLock(SlimLock& s_lock) noexcept
			: mp_lock(&s_lock)
		{
			mp_lock->lock();
		}

		// Move-construct AutoLock
		AutoLock(AutoLock&& other) noexcept
			: mp_lock(other.mp_lock)
		{
			other.mp_lock = nullptr;
		}

		// Destroy AutoLock
		~AutoLock() noexcept
		{
			if (mp_lock)
			{
				mp_lock->unlock();
			}
		}

		// Move-assign AutoLock
		AutoLock& operator=(AutoLock&& other) noexcept
		{
			mp_lock->unlock();
			mp_lock = other.mp_lock;
			other.mp_lock = nullptr;
			return *this;
		}

	private:

		SlimLock* mp_lock;

	};

	class ReadGuard final
	{
	public:

		// Construct ReadGuard
		explicit ReadGuard(CounterType& counter) noexcept
			: mp_counter(&counter)
		{
			++(*mp_counter);
		}

		// Move-construct ReadGuard
		ReadGuard(ReadGuard&& other) noexcept
			: mp_counter(other.mp_counter)
		{
			other.mp_counter = nullptr;
		}

		// Destroy ReadGuard
		~ReadGuard() noexcept
		{
			if (mp_counter)
			{
				--(*mp_counter);
			}
		}

		// Move-assign ReadGuard
		ReadGuard& operator=(ReadGuard&& other) noexcept
		{
			--(*mp_counter);
			mp_counter = other.mp_counter;
			other.mp_counter = nullptr;
			return *this;
		}

	private:

		CounterType* mp_counter;

	};

	// Access Signal's internal structure for reading
	ReadGuard read_access() const noexcept
	{
		return ReadGuard(m_SyncStage.load() ==
			SyncStage::SyncStage_1 ? m_access_s1 : m_access_s2);
	}

	// Access Signal's internal structure for writing
	AutoLock write_access() const noexcept
	{
		return AutoLock(m_write_lock);
	}

	// Synchronize internal Signal's state - delete logically removed
	// elements according to synchronized stage (if possible).
	// Must be called under write_access() protection.
	void synchronize() noexcept
	{
		ConnectionType** first = nullptr;
		ConnectionType* to_delete = nullptr;

		SyncStage current_stage = m_SyncStage.load();

		switch (current_stage)
		{
		case SyncStage::SyncStage_1:
			if (!m_access_s2.load())
			{
				to_delete = mp_deleted_s2;
				first = &mp_deleted_s2;
				current_stage = SyncStage::SyncStage_2;
				break;
			}
			else
			{
				return;
			}
		case SyncStage::SyncStage_2:
			if (!m_access_s1.load())
			{
				to_delete = mp_deleted_s1;
				first = &mp_deleted_s1;
				current_stage = SyncStage::SyncStage_1;
				break;
			}
			else
			{
				return;
			}
		}

		while (to_delete)
		{
			(*first) = to_delete->mDeletedPtr;
			to_delete->~ConnectionType();
			m_Storage.deallocate(to_delete);
			to_delete = (*first);
		}

		m_SyncStage.store(current_stage);
	}

	// Fix pointers of removed elements to point after this element.
	// Must be called under write_access() protection.
	void fix_pointers(ConnectionType * node, ConnectionType * removed) noexcept
	{
		while (removed)
		{
			if (removed->mNextPtr.load() == node)
			{
				removed->mNextPtr.store(node->mNextPtr.load());
			}
			removed = removed->mDeletedPtr;
		}
	}

	// Logically remove element from ConnectionType list.
	// Must be called under write_access() protection.
	void remove(ConnectionType * node) noexcept
	{
		ConnectionType** previous = nullptr;
		ConnectionType* current = nullptr;

		switch (m_SyncStage.load())
		{
		case SyncStage::SyncStage_1:
			previous = &mp_deleted_s1;
			current = mp_deleted_s1;
			break;
		case SyncStage::SyncStage_2:
			previous = &mp_deleted_s2;
			current = mp_deleted_s2;
			break;
		}

		fix_pointers(node, mp_deleted_s1);
		fix_pointers(node, mp_deleted_s2);

		while (current)
		{
			previous = &current->mDeletedPtr;
			current = current->mDeletedPtr;
		}

		(*previous) = node;
	}

	// Logically remove all elements from ConnectionType list.
	// Must be called under write_access() protection.
	void remove_all() noexcept
	{
		ConnectionType* to_delete = mp_first_slot.load();

		if (to_delete)
		{
			while (to_delete)
			{
				to_delete->mDeletedPtr = to_delete->mNextPtr.load();
				to_delete->mNextPtr.store(nullptr);
				to_delete = to_delete->mDeletedPtr;
			}

			remove(mp_first_slot.load());
			mp_first_slot.store(nullptr);
		}
	}

	// Delete logically removed elements regardless of synchronization.
	// Must be called under write_access() protection.
	void clear(ConnectionType * removed) noexcept
	{
		while (removed)
		{
			ConnectionType* to_delete = removed;
			removed = removed->mDeletedPtr;
			to_delete->~ConnectionType();
			m_Storage.deallocate(to_delete);
		}
	}

	// Connect new slot to the Signal
	// May throw exception if memory allocation fails
	// Must be called under write_access() protection
	bool connect(const Slot<ReType(Args...)> & slot,
		const TrackPtr & t_ptr,
		bool trackable)
	{
		synchronize();

		ConnectionType* current = mp_first_slot.load();
		ConnectionPtr* previous = &mp_first_slot;

		while (current)
		{
			if (current->mSlot == slot)
			{
				return false;
			}
			else
			{
				previous = &current->mNextPtr;
				current = current->mNextPtr.load();
			}
		}

		ConnectionType* new_Connection = m_Storage.allocate();
		::new(new_Connection) ConnectionType(slot, t_ptr, trackable);
		previous->store(new_Connection);
		return true;
	}

	// Disconnect slot from the Signal
	// Must be called under write_access() protection
	bool disconnect(const Slot<ReType(Args...)> & slot) noexcept
	{
		synchronize();

		ConnectionType* current = mp_first_slot.load();
		ConnectionPtr* previous = &mp_first_slot;

		while (current)
		{
			if (current->mSlot == slot)
			{
				previous->store(current->mNextPtr.load());
				remove(current);
				return true;
			}
			else
			{
				previous = &current->mNextPtr;
				current = current->mNextPtr.load();
			}
		}

		return false;
	}

	// Check whether slot is connected to the Signal
	// Must be called under read_access() protection
	bool connected(const Slot<ReType(Args...)> & slot) const noexcept
	{
		ConnectionType* current = mp_first_slot.load();

		while (current)
		{
			if (current->mSlot == slot)
			{
				return true;
			}
			else
			{
				current = current->mNextPtr.load();
			}
		}

		return false;
	}

	// Activate Signal
	// May throw exception if some slot does
	// Must be called under read_access() protection
	void activate(Args&& ... args)
	{
		ConnectionType* current = mp_first_slot.load();

		while (current)
		{
			if (!current->mTrackable)
			{
				current->mSlot(::std::forward<Args>(args)...);
				current = current->mNextPtr.load();
			}
			else
			{
				auto ptr = current->mTrackPtr.lock();

				if (ptr)
				{
					current->mSlot(::std::forward<Args>(args)...);
					current = current->mNextPtr.load();
				}
				else
				{
					ConnectionType* to_delete = current;
					current = current->mNextPtr.load();
					auto writer = write_access();
					disconnect(to_delete->mSlot);
				}
			}
		}
	}

public:

	// Construct Signal with provided / default capacity
	// May throw exception if memory allocation fails
	explicit Signal(size_type capacity = 5)
		: m_Storage{ capacity }
		, mp_first_slot{ nullptr }
		, mp_deleted_s1{ nullptr }
		, mp_deleted_s2{ nullptr }
		, m_access_s1{ 0 }
		, m_access_s2{ 0 }
		, m_write_lock{}
		, m_SyncStage{ JeJo::SyncStage::SyncStage_1 }
		, m_blocked{ false }
	{}

	// Deleted copy-constructor
	constexpr Signal(const Signal&) noexcept = delete;

	// Destroy Signal
	~Signal() noexcept
	{
		const auto writer{ this->write_access() };
		this->remove_all();
		this->clear(mp_deleted_s1);
		this->clear(mp_deleted_s2);
	}

	// Deleted copy-assignment operator
	constexpr Signal& operator=(const Signal&) noexcept = delete;

	// Connect Signal to slot (static method / free function)
	// May throw exception if memory allocation fails
	bool connect(ReType(*function)(Args...))
	{
		const auto writer{ write_access() };
		return connect(Slot<ReType(Args...)>(function), TrackPtr(), false);
	}

	// Connect Signal to slot (method)
	// May throw exception if memory allocation fails
	template<typename ClassType, typename FunctionPtrType>
	bool connect(ClassType* object, FunctionPtrType method)
	{
		auto writer = write_access();
		return connect(Slot<ReType(Args...)>(object, method), TrackPtr(), false);
	}

	// Connect Signal to traceable slot (method)
	// May throw exception if memory allocation fails
	template<typename ClassType, typename FunctionPtrType>
	bool connect(::std::shared_ptr<ClassType> object, FunctionPtrType method)
	{
		auto writer = write_access();
		return connect(Slot<ReType(Args...)>(object.get(), method), TrackPtr(object), true);
	}

	// Connect Signal to slot (functor)
	// May throw exception if memory allocation fails
	template<typename ClassType>
	bool connect(ClassType* functor)
	{
		auto writer = write_access();
		return connect(Slot<ReType(Args...)>(functor), TrackPtr(), false);
	}

	// Connect Signal to traceable slot (functor)
	// May throw exception if memory allocation fails
	template<typename ClassType>
	bool connect(::std::shared_ptr<ClassType> functor)
	{
		auto writer = write_access();
		return connect(Slot<ReType(Args...)>(functor.get()), TrackPtr(functor), true);
	}

	// Disconnect Signal from slot (static method / free function)
	bool disconnect(ReType(*function)(Args...)) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot<ReType(Args...)>(function));
	}

	// Disconnect Signal from slot (method)
	template<typename ClassType, typename FunctionPtrType>
	bool disconnect(ClassType* object, FunctionPtrType method) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot<ReType(Args...)>(object, method));
	}

	// Disconnect Signal from traceable slot (method)
	template<typename ClassType, typename FunctionPtrType>
	bool disconnect(::std::shared_ptr<ClassType> object, FunctionPtrType method) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot<ReType(Args...)>(object.get(), method));
	}

	// Disconnect Signal from slot (functor)
	template<typename ClassType>
	bool disconnect(ClassType* functor) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot<ReType(Args...)>(functor));
	}

	// Disconnect Signal from track-able slot (functor)
	template<typename ClassType>
	bool disconnect(::std::shared_ptr<ClassType> functor) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot<ReType(Args...)>(functor.get()));
	}

	// Disconnect Signal from all slots
	void disconnect_all() noexcept
	{
		auto writer = write_access();
		remove_all();
	}

	// Check whether slot is connected (static method / free function)
	bool connected(ReType(*function)(Args...)) const noexcept
	{
		auto reader = read_access();
		return connected(Slot<ReType(Args...)>(function));
	}

	// Check whether slot is connected (method)
	template<typename ClassType, typename FunctionPtrType>
	bool connected(ClassType* object, FunctionPtrType method) const noexcept
	{
		auto reader = read_access();
		return connected(Slot<ReType(Args...)>(object, method));
	}

	// Check whether traceable slot is connected (method)
	template<typename ClassType, typename FunctionPtrType>
	bool connected(::std::shared_ptr<ClassType> object, FunctionPtrType method) const noexcept
	{
		auto reader = read_access();
		return connected(Slot<ReType(Args...)>(object.get(), method));
	}

	// Check whether slot is connected (functor)
	template<typename ClassType>
	bool connected(ClassType* functor) const noexcept
	{
		auto reader = read_access();
		return connected(Slot<ReType(Args...)>(functor));
	}

	// Check whether traceable slot is connected (functor)
	template<typename ClassType>
	bool connected(::std::shared_ptr<ClassType> functor) const noexcept
	{
		auto reader = read_access();
		return connected(Slot<ReType(Args...)>(functor.get()));
	}

	// Block Signal
	void block(bool block = true) noexcept
	{
		m_blocked.store(block);
	}

	// Check whether Signal is blocked
	bool blocked() const noexcept
	{
		return m_blocked.load();
	}

	// Emit Signal
	// May throw exception if some slot does
	void emit(Args&&... args)
	{
		auto reader = read_access();
		if (!m_blocked.load())
		{
			activate(::std::forward<Args>(args)...);
		}
	}

	// Emit Signal
	// May throw exception if some slot does
	void operator()(Args ... args)
	{
		auto reader = read_access();
		if (!m_blocked.load())
		{
			activate(args...);
		}
	}

	// Get number of connected slots
	size_type size() const noexcept
	{
		auto writer = write_access();
		ConnectionType* current = mp_first_slot.load();
		size_type size = 0;

		while (current)
		{
			++size;
			current = current->mNextPtr.load();
		}

		return size;
	}

	// Check whether list of connected slots is empty
	bool empty() const noexcept
	{
		return !mp_first_slot.load();
	}

private:

	Storage<ReType(Args...)>	m_Storage;
	::std::atomic<Connection<ReType(Args...)>*>			mp_first_slot;
	ConnectionType* mp_deleted_s1;
	ConnectionType* mp_deleted_s2;
	mutable CounterType	m_access_s1;
	mutable CounterType	m_access_s2;
	mutable SlimLock		m_write_lock;
	AccessStage			m_SyncStage;
	AtomicBoolType				m_blocked;

};

JEJO_END

#endif // JEJO_SIGNALS_T_HPP

/*****************************************************************************/