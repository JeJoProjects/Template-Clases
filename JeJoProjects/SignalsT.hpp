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
#include <utility>		// std::forward<>()
#include <new>			// new()
#include <memory>		// std::shared_ptr<>, std::weak_ptr<>

// own JeJo-lib headers
#include "SlotT.hpp"
#include "StorageT.hpp"
#include "LockClassesT.hpp"


// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

JEJO_BEGIN

using namespace internal;

// TEMPLATE CLASS Signal
template<typename ReType, typename... Args> class Signal;

template<typename ReType, typename... Args> class Signal<ReType(Args...)> final
{
private:
	using Connection = Connection<ReType(Args...)>;
	using ConnectionPtr = Connection*;
	using AtomicConnectionPtr = std::atomic<ConnectionPtr>;

	Storage<ReType(Args...)>	m_Storage;
	AtomicConnectionPtr mp_first_slot;
	ConnectionPtr mp_deleted_s1;
	ConnectionPtr mp_deleted_s2;
	mutable CounterType	m_access_s1;
	mutable CounterType	m_access_s2;
	mutable SlimLock		m_write_lock;
	AccessStage			m_SyncStage;
	AtomicBoolType				m_blocked;

private:
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
		ConnectionPtr* first = nullptr;
		ConnectionPtr to_delete = nullptr;

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
			to_delete->~Connection();
			m_Storage.deallocate(to_delete);
			to_delete = (*first);
		}

		m_SyncStage.store(current_stage);
	}

	// Fix pointers of removed elements to point after this element.
	// Must be called under write_access() protection.
	void fix_pointers(Connection * node, Connection * removed) noexcept
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

	// Logically remove element from Connection list.
	// Must be called under write_access() protection.
	void remove(Connection * node) noexcept
	{
		ConnectionPtr* previous = nullptr;
		ConnectionPtr current = nullptr;

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

	// Logically remove all elements from Connection list.
	// Must be called under write_access() protection.
	void remove_all() noexcept
	{
		ConnectionPtr to_delete = mp_first_slot.load();

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
	void clear(Connection * removed) noexcept
	{
		while (removed)
		{
			ConnectionPtr to_delete = removed;
			removed = removed->mDeletedPtr;
			to_delete->~Connection();
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

		ConnectionPtr current = mp_first_slot.load();
		AtomicConnectionPtr* previous = &mp_first_slot;

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

		ConnectionPtr new_Connection = m_Storage.allocate();
		::new(new_Connection) Connection(slot, t_ptr, trackable);
		previous->store(new_Connection);
		return true;
	}

	// Disconnect slot from the Signal
	// Must be called under write_access() protection
	bool disconnect(const Slot<ReType(Args...)> & slot) noexcept
	{
		synchronize();

		ConnectionPtr current = mp_first_slot.load();
		AtomicConnectionPtr* previous = &mp_first_slot;

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
		ConnectionPtr current = mp_first_slot.load();

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
		ConnectionPtr current = mp_first_slot.load();

		while (current)
		{
			if (!current->mTrackable)
			{
				current->mSlot(std::forward<Args>(args)...);
				current = current->mNextPtr.load();
			}
			else
			{
				auto ptr = current->mTrackPtr.lock();

				if (ptr)
				{
					current->mSlot(std::forward<Args>(args)...);
					current = current->mNextPtr.load();
				}
				else
				{
					ConnectionPtr to_delete = current;
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
		, m_SyncStage{ SyncStage::SyncStage_1 }
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
	bool connect(std::shared_ptr<ClassType> object, FunctionPtrType method)
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
	bool connect(std::shared_ptr<ClassType> functor)
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
	bool disconnect(std::shared_ptr<ClassType> object, FunctionPtrType method) noexcept
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
	bool disconnect(std::shared_ptr<ClassType> functor) noexcept
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
	bool connected(std::shared_ptr<ClassType> object, FunctionPtrType method) const noexcept
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
	bool connected(std::shared_ptr<ClassType> functor) const noexcept
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
			activate(std::forward<Args>(args)...);
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
		ConnectionPtr current = mp_first_slot.load();
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
};

JEJO_END

#endif // JEJO_SIGNALS_T_HPP

/*****************************************************************************/