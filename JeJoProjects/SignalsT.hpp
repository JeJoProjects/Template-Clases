#ifndef SignalS_T_HPP
#define SignalS_T_HPP

#include <cstddef>		// std::size_t, std::nullptr_t
#include <utility>		// std::move(), std::forward<>()
#include <algorithm>	// std::copy()
#include <new>			// ::new()
#include <atomic>		// std::atomic<>, std::atomic_flag
#include <memory>		// std::shared_ptr<>, std::weak_ptr<>
#include <thread>		// std::this_thread


// Macros for dynamic memory allocation
#define NEW_MEMORY(arg) ::operator new(arg)
#define DELETE_MEMORY(arg) ::operator delete(arg)

// TEMPLATE CLASS Signal
template<typename ResT, typename ... ArgTs>
class Signal;

template<typename ResT, typename ... ArgTs>
class Signal<ResT(ArgTs...)> final
{
	// Unified function pointer wrapper. Instances of SlotFunctor
	// can store, copy, and invoke any callable target (slot).
	class SlotFunctor;

	// Node in linked list of connected slots. Contains information
	// related to a particular Connection between a Signal and a slot.
	class Connection;

	// Memory manager for Signal object. Keeps memory blocks to store
	// Connection objects. Provides memory allocations / deallocations.
	class Storage;

	// Synchronization primitive that protects shared data from being
	// simultaneously modified by multiple threads. Optimized for speed
	// and occupies very little memory. Meets Lockable requirements.
	class SlimLock;

	// Class AutoLock is a SlimLock ownership wrapper that provides a
	// convenient RAII-style mechanism for automatic locking / unlocking.
	class AutoLock;

	// Class ReadGuard is a stage counter ownership wrapper. Provides
	// a convenient RAII-style mechanism for automatic increm / decrem.
	class ReadGuard;

	// Synchronization stage. Specifies current access stage.
	enum class SyncStage : char { SyncStage_1 = 1, SyncStage_2 = 2 };

	using Byte				= unsigned char;
	using size_type			= ::std::size_t;
	using AtomicBoolType	= ::std::atomic<bool>;
	using CounterType		= ::std::atomic<unsigned short>;
	using AccessStage		= ::std::atomic<SyncStage>;
	using ConnectionPtr	    = ::std::atomic<Connection*>;
	using TrackPtr			= ::std::weak_ptr<void>;

	class SlotFunctor final
	{
		// Structure to store target pointers
		template<typename Class, typename Signature>
		struct TargetSlot final
		{
			using SlotFunction = Signature;
			using SlotInstance = Class *;
			SlotFunction mp_function;
			SlotInstance mp_instance;
		};

		// Unknown default class (undefined)
		class DefaultClass;

		// Unknown default function (undefined)
		using DefaultFunction = void(DefaultClass::*)(void);

		// Default TargetSlot type
		using DefaultType = TargetSlot<DefaultClass, DefaultFunction>;

		// Size of default target data
		static const size_type target_size = sizeof(DefaultType);

		// Storage for target data
		using SlotStorage = Byte[target_size];

		// Type of invoker-function
		using InvokerType = ResT(*)(const Byte* const, ArgTs& ...);

		// Invoke target slot (static method / free function)
		template<::std::nullptr_t, typename Signature>
		static ResT invoke(const Byte* const data, ArgTs& ... args)
		{
			return
				(*reinterpret_cast<const TargetSlot<::std::nullptr_t, Signature>*>
				(data)->mp_function)(args...);
		}

		// Invoke target slot (method)
		template<typename Class, typename Signature>
		static ResT invoke(const Byte* const data, ArgTs& ... args)
		{
			return
				(reinterpret_cast<const TargetSlot<Class, Signature>*>
				(data)->mp_instance->*
					reinterpret_cast<const TargetSlot<Class, Signature>*>
					(data)->mp_function)(args...);
		}

		// Invoke target slot (functor)
		template<typename Class, ::std::nullptr_t>
		static ResT invoke(const Byte* const data, ArgTs& ... args)
		{
			return
				(*reinterpret_cast<const TargetSlot<Class, ::std::nullptr_t>*>
				(data)->mp_instance)(args...);
		}

	public:

		// Construct SlotFunctor (static method / free function)
		explicit SlotFunctor(ResT(*function)(ArgTs...)) noexcept
			: m_target{}, mp_invoker{ nullptr }
		{
			using Signature = decltype(function);
			auto Storage = reinterpret_cast<TargetSlot
				<::std::nullptr_t, Signature>*>(&m_target[0]);
			Storage->mp_function = function;
			Storage->mp_instance = nullptr;
			mp_invoker = &SlotFunctor::invoke<nullptr, Signature>;
		}

		// Construct SlotFunctor (method)
		template<typename Class, typename Signature>
		SlotFunctor(Class* object, Signature method) noexcept
			: m_target{}, mp_invoker{ nullptr }
		{
			auto Storage = reinterpret_cast<TargetSlot
				<Class, Signature>*>(&m_target[0]);
			Storage->mp_function = method;
			Storage->mp_instance = object;
			mp_invoker = &SlotFunctor::invoke<Class, Signature>;
		}

		// Construct SlotFunctor (functor)
		template<typename Class>
		explicit SlotFunctor(Class* functor) noexcept
			: m_target{}, mp_invoker{ nullptr }
		{
			auto Storage = reinterpret_cast<TargetSlot
				<Class, ::std::nullptr_t>*>(&m_target[0]);
			Storage->mp_function = nullptr;
			Storage->mp_instance = functor;
			mp_invoker = &SlotFunctor::invoke<Class, nullptr>;
		}

		// Deleted constructor (null pointer)
		SlotFunctor(::std::nullptr_t)noexcept = delete;

		// Copy-construct SlotFunctor
		SlotFunctor(const SlotFunctor& other) noexcept
		{
			::std::copy(other.m_target,
				other.m_target + target_size, m_target);
			mp_invoker = other.mp_invoker;
		}

		// Destroy SlotFunctor
		~SlotFunctor() noexcept
		{}

		// Copy-assign SlotFunctor
		SlotFunctor& operator=(const SlotFunctor& other) noexcept
		{
			if (this != &other)
			{
				::std::copy(other.m_target,
					other.m_target + target_size, m_target);
				mp_invoker = other.mp_invoker;
			}
			return *this;
		}

		// Invoke target slot
		ResT operator()(ArgTs& ... args) const
		{
			return (*mp_invoker)(&m_target[0], args...);
		}

		// Compare slot_functors (equal)
		bool operator==(const SlotFunctor& other) const noexcept
		{
			for (size_type index = 0; index < target_size; ++index)
			{
				if (m_target[index] != other.m_target[index])
				{
					return false;
				}
			}
			return true;
		}

		// Compare slot_functors (not equal)
		bool operator!=(const SlotFunctor& other) const noexcept
		{
			for (size_type index = 0; index < target_size; ++index)
			{
				if (m_target[index] != other.m_target[index])
				{
					return true;
				}
			}
			return false;
		}

	private:

		alignas(DefaultType)SlotStorage m_target;
		alignas(InvokerType)InvokerType mp_invoker;

	};

	class Connection final
	{
	public:

		// Construct Connection
		Connection(const SlotFunctor& slot,
			const TrackPtr& t_ptr,
			bool trackable) noexcept
			: m_slot(slot),
			m_track_ptr(t_ptr),
			mp_next(nullptr),
			mp_deleted(nullptr),
			m_trackable(trackable)
		{}

		// Deleted copy-constructor
		Connection(const Connection&)noexcept = delete;

		// Destroy Connection
		~Connection() noexcept
		{}

		// Deleted copy-assignment operator
		Connection& operator=(const Connection&)noexcept = delete;

	public:

		SlotFunctor	m_slot;
		TrackPtr		m_track_ptr;
		ConnectionPtr	mp_next;
		Connection* mp_deleted;
		const bool		m_trackable;

	};

	class Storage final
	{
		// Initialize requested memory block
		void init(Connection* address) noexcept
		{
			mp_store = address;
			Connection** current = reinterpret_cast<Connection * *>(address);

			for (size_type index = 1; index < m_capacity; ++index)
			{
				(*current) = ++address;
				current = reinterpret_cast<Connection * *>(address);
			}

			(*current) = nullptr;
		}

		// Expand memory by allocating new memory block
		// May throw exception if memory allocation fails
		void expand()
		{
			Connection* new_block = reinterpret_cast<Connection*>
				(NEW_MEMORY(sizeof(Connection) * m_capacity +
					sizeof(Connection*)));
			Connection * *next_block = reinterpret_cast<Connection * *>
				(mp_block + m_capacity);

			while (*next_block)
			{
				next_block = reinterpret_cast<Connection * *>
					((*next_block) + m_capacity);
			}

			(*next_block) = new_block;
			init(new_block);
			(*reinterpret_cast<Connection * *>
				(new_block + m_capacity)) = nullptr;
		}

		// Free all allocated memory blocks
		void clear() noexcept
		{
			if (mp_block)
			{
				Connection* to_delete = (*reinterpret_cast<Connection * *>
					(mp_block + m_capacity));

				while (to_delete)
				{
					Connection* block = (*reinterpret_cast<Connection * *>
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
		Storage(size_type capacity)
			: mp_block(nullptr),
			mp_store(nullptr),
			m_capacity(capacity >= 1 ? capacity : 1)
		{
			mp_block = reinterpret_cast<Connection*>(NEW_MEMORY(m_capacity
				* sizeof(Connection) + sizeof(Connection*)));
			init(mp_block);
			(*reinterpret_cast<Connection * *>
				(mp_block + m_capacity)) = nullptr;
		}

		// Move-construct Storage
		Storage(Storage && other) noexcept
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
		Storage& operator=(Storage && other) noexcept
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
		Connection* allocate()
		{
			if (!mp_store)
			{
				expand(); // May throw
			}

			Connection* current = mp_store;
			mp_store = (*reinterpret_cast<Connection * *>(mp_store));
			return current;
		}

		// Deallocate previously allocated memory
		void deallocate(Connection * address) noexcept
		{
			(*reinterpret_cast<Connection * *>(address)) = mp_store;
			mp_store = address;
		}

	private:

		Connection* mp_block;
		Connection* mp_store;
		size_type		m_capacity;

	};

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
		Connection** first = nullptr;
		Connection* to_delete = nullptr;

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
			(*first) = to_delete->mp_deleted;
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
			if (removed->mp_next.load() == node)
			{
				removed->mp_next.store(node->mp_next.load());
			}
			removed = removed->mp_deleted;
		}
	}

	// Logically remove element from Connection list.
	// Must be called under write_access() protection.
	void remove(Connection * node) noexcept
	{
		Connection** previous = nullptr;
		Connection* current = nullptr;

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
			previous = &current->mp_deleted;
			current = current->mp_deleted;
		}

		(*previous) = node;
	}

	// Logically remove all elements from Connection list.
	// Must be called under write_access() protection.
	void remove_all() noexcept
	{
		Connection* to_delete = mp_first_slot.load();

		if (to_delete)
		{
			while (to_delete)
			{
				to_delete->mp_deleted = to_delete->mp_next.load();
				to_delete->mp_next.store(nullptr);
				to_delete = to_delete->mp_deleted;
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
			Connection* to_delete = removed;
			removed = removed->mp_deleted;
			to_delete->~Connection();
			m_Storage.deallocate(to_delete);
		}
	}

	// Connect new slot to the Signal
	// May throw exception if memory allocation fails
	// Must be called under write_access() protection
	bool connect(const SlotFunctor & slot,
		const TrackPtr & t_ptr,
		bool trackable)
	{
		synchronize();

		Connection* current = mp_first_slot.load();
		ConnectionPtr* previous = &mp_first_slot;

		while (current)
		{
			if (current->m_slot == slot)
			{
				return false;
			}
			else
			{
				previous = &current->mp_next;
				current = current->mp_next.load();
			}
		}

		Connection* new_Connection = m_Storage.allocate();
		::new(new_Connection) Connection(slot, t_ptr, trackable);
		previous->store(new_Connection);
		return true;
	}

	// Disconnect slot from the Signal
	// Must be called under write_access() protection
	bool disconnect(const SlotFunctor & slot) noexcept
	{
		synchronize();

		Connection* current = mp_first_slot.load();
		ConnectionPtr* previous = &mp_first_slot;

		while (current)
		{
			if (current->m_slot == slot)
			{
				previous->store(current->mp_next.load());
				remove(current);
				return true;
			}
			else
			{
				previous = &current->mp_next;
				current = current->mp_next.load();
			}
		}

		return false;
	}

	// Check whether slot is connected to the Signal
	// Must be called under read_access() protection
	bool connected(const SlotFunctor & slot) const noexcept
	{
		Connection* current = mp_first_slot.load();

		while (current)
		{
			if (current->m_slot == slot)
			{
				return true;
			}
			else
			{
				current = current->mp_next.load();
			}
		}

		return false;
	}

	// Activate Signal
	// May throw exception if some slot does
	// Must be called under read_access() protection
	void activate(ArgTs & ... args)
	{
		Connection* current = mp_first_slot.load();

		while (current)
		{
			if (!current->m_trackable)
			{
				current->m_slot(args...);
				current = current->mp_next.load();
			}
			else
			{
				auto ptr = current->m_track_ptr.lock();

				if (ptr)
				{
					current->m_slot(args...);
					current = current->mp_next.load();
				}
				else
				{
					Connection* to_delete = current;
					current = current->mp_next.load();
					auto writer = write_access();
					disconnect(to_delete->m_slot);
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
	bool connect(ResT(*function)(ArgTs...))
	{
		const auto writer{ write_access() };
		return connect(SlotFunctor(function), TrackPtr(), false);
	}

	// Connect Signal to slot (method)
	// May throw exception if memory allocation fails
	template<typename Class, typename Signature>
	bool connect(Class * object, Signature method)
	{
		auto writer = write_access();
		return connect(SlotFunctor(object, method), TrackPtr(), false);
	}

	// Connect Signal to traceable slot (method)
	// May throw exception if memory allocation fails
	template<typename Class, typename Signature>
	bool connect(::std::shared_ptr<Class> object, Signature method)
	{
		auto writer = write_access();
		return connect(SlotFunctor(object.get(), method), TrackPtr(object), true);
	}

	// Connect Signal to slot (functor)
	// May throw exception if memory allocation fails
	template<typename Class>
	bool connect(Class * functor)
	{
		auto writer = write_access();
		return connect(SlotFunctor(functor), TrackPtr(), false);
	}

	// Connect Signal to traceable slot (functor)
	// May throw exception if memory allocation fails
	template<typename Class>
	bool connect(::std::shared_ptr<Class> functor)
	{
		auto writer = write_access();
		return connect(SlotFunctor(functor.get()), TrackPtr(functor), true);
	}

	// Disconnect Signal from slot (static method / free function)
	bool disconnect(ResT(*function)(ArgTs...)) noexcept
	{
		auto writer = write_access();
		return disconnect(SlotFunctor(function));
	}

	// Disconnect Signal from slot (method)
	template<typename Class, typename Signature>
	bool disconnect(Class * object, Signature method) noexcept
	{
		auto writer = write_access();
		return disconnect(SlotFunctor(object, method));
	}

	// Disconnect Signal from traceable slot (method)
	template<typename Class, typename Signature>
	bool disconnect(::std::shared_ptr<Class> object, Signature method) noexcept
	{
		auto writer = write_access();
		return disconnect(SlotFunctor(object.get(), method));
	}

	// Disconnect Signal from slot (functor)
	template<typename Class>
	bool disconnect(Class * functor) noexcept
	{
		auto writer = write_access();
		return disconnect(SlotFunctor(functor));
	}

	// Disconnect Signal from track-able slot (functor)
	template<typename Class>
	bool disconnect(::std::shared_ptr<Class> functor) noexcept
	{
		auto writer = write_access();
		return disconnect(SlotFunctor(functor.get()));
	}

	// Disconnect Signal from all slots
	void disconnect_all() noexcept
	{
		auto writer = write_access();
		remove_all();
	}

	// Check whether slot is connected (static method / free function)
	bool connected(ResT(*function)(ArgTs...)) const noexcept
	{
		auto reader = read_access();
		return connected(SlotFunctor(function));
	}

	// Check whether slot is connected (method)
	template<typename Class, typename Signature>
	bool connected(Class * object, Signature method) const noexcept
	{
		auto reader = read_access();
		return connected(SlotFunctor(object, method));
	}

	// Check whether traceable slot is connected (method)
	template<typename Class, typename Signature>
	bool connected(::std::shared_ptr<Class> object, Signature method) const noexcept
	{
		auto reader = read_access();
		return connected(SlotFunctor(object.get(), method));
	}

	// Check whether slot is connected (functor)
	template<typename Class>
	bool connected(Class * functor) const noexcept
	{
		auto reader = read_access();
		return connected(SlotFunctor(functor));
	}

	// Check whether traceable slot is connected (functor)
	template<typename Class>
	bool connected(::std::shared_ptr<Class> functor) const noexcept
	{
		auto reader = read_access();
		return connected(SlotFunctor(functor.get()));
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
	void emit(ArgTs ... args)
	{
		auto reader = read_access();
		if (!m_blocked.load())
		{
			activate(args...);
		}
	}

	// Emit Signal
	// May throw exception if some slot does
	void operator()(ArgTs ... args)
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
		Connection* current = mp_first_slot.load();
		size_type size = 0;

		while (current)
		{
			++size;
			current = current->mp_next.load();
		}

		return size;
	}

	// Check whether list of connected slots is empty
	bool empty() const noexcept
	{
		return !mp_first_slot.load();
	}

private:

	Storage					m_Storage;
	ConnectionPtr			mp_first_slot;
	Connection* mp_deleted_s1;
	Connection* mp_deleted_s2;
	mutable CounterType	m_access_s1;
	mutable CounterType	m_access_s2;
	mutable SlimLock		m_write_lock;
	AccessStage			m_SyncStage;
	AtomicBoolType				m_blocked;

};

#endif // SignalS_T_HPP

#if 0
// Construct an object of type _Ty, passing std::shared_ptr<_Ty> and
// args as arguments to the constructor of _Ty.
// Allocates memory using ::operator new.
// May throw std::bad_alloc exception if memory allocation fails.
// _Ty's constructor must not throw.
template<typename _Ty, typename ... ArgTs>
::std::shared_ptr<_Ty> create_shared_object(ArgTs && ... args)
{
	_Ty* address = static_cast<_Ty*>(NEW_MEMORY(sizeof(_Ty)));
	::std::shared_ptr<_Ty> sh_ptr(address,
		[](_Ty * ptr) { ptr->~_Ty(); DELETE_MEMORY(ptr); });
	::new (address) _Ty(sh_ptr, ::std::forward<ArgTs>(args)...);
	return ::std::move(sh_ptr);
}

// Construct an object of type _Ty, passing std::shared_ptr<_Ty> and
// args as arguments to the constructor of _Ty.
// Allocates memory using provided allocator alloc.
// alloc must satisfy C++ Allocator concept requirements.
// May throw std::bad_alloc exception if memory allocation fails.
// _Ty's constructor must not throw.
template<typename _Ty, typename _Alloc, typename ... ArgTs>
::std::shared_ptr<_Ty> allocate_shared_object(const _Alloc & alloc, ArgTs && ... args)
{
	_Alloc _allocator(alloc);
	_Ty* address = _allocator.allocate(1);
	::std::shared_ptr<_Ty> sh_ptr(address,
		[_allocator](_Ty * ptr) mutable
		{ _allocator.destroy(ptr); _allocator.deallocate(ptr, 1); }, _allocator);
	_allocator.construct(address, sh_ptr, ::std::forward<ArgTs>(args)...);
	return ::std::move(sh_ptr);
}
#endif