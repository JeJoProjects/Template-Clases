/******************************************************************************
 * Implementation of Signal<>, which provides a template 
 *
 * @Authur :  JeJo
 * @Date   :  June - 2021
 * @license: free to use and distribute(no further support as well)
 *
 * @todo: explanation and limitations of the class
 *****************************************************************************/

#ifndef JEJO_SignalS_T_HPP
#define JEJO_SignalS_T_HPP

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

// macros for name-spacing
#define JEJO_BEGIN namespace JeJo {
#define JEJO_END   }

// Macros for dynamic memory allocation
#define NEW_MEMORY(arg) ::operator new(arg)
#define DELETE_MEMORY(arg) ::operator delete(arg)

JEJO_BEGIN

// TEMPLATE CLASS Signal
template<typename ReType, typename... Args>
class Signal;

template<typename ReType, typename... Args>
class Signal<ReType(Args...)> final
{
	// Unified function pointer wrapper. Instances of Slot
	// can store, copy, and invoke any callable target (slot).
	class Slot;

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

	// ClassType AutoLock is a SlimLock ownership wrapper that provides a
	// convenient RAII-style mechanism for automatic locking / unlocking.
	class AutoLock;

	// ClassType ReadGuard is a stage counter ownership wrapper. Provides
	// a convenient RAII-style mechanism for automatic increm / decrem.
	class ReadGuard;

	// Synchronization stage. Specifies current access stage.
	enum struct SyncStage : char { SyncStage_1 = 1, SyncStage_2 = 2 };

	using Byte				= unsigned char;
	using size_type			= ::std::size_t;
	using AtomicBoolType	= ::std::atomic<bool>;
	using CounterType		= ::std::atomic<unsigned short>;
	using AccessStage		= ::std::atomic<SyncStage>;
	using ConnectionPtr	    = ::std::atomic<Connection*>;
	using TrackPtr			= ::std::weak_ptr<void>;

	class Slot final
	{
	private:
		// Structure to store target pointers
		template<typename ClassType, typename FunctionPtrType>
		struct TargetSlot final
		{
			ClassType* mClassInstance;
			FunctionPtrType mFunctionPtr;
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
		using SlotStorage = ::std::array<Byte, target_size>;

		// Type of invoker-function
		using InvokerType = ReType(*)(const Byte* const, Args&&...);

		alignas(DefaultType)SlotStorage mTarget;
		alignas(InvokerType)InvokerType mInvoker;

		// Invoke target slot (static method / free function)
		template<::std::nullptr_t, typename FunctionPtrType>
		static ReType invoke(const Byte* const data, Args&&... args)
		{
			return
				(*reinterpret_cast<const TargetSlot<::std::nullptr_t, FunctionPtrType>*>
				(data)->mFunctionPtr)(::std::forward<Args>(args)...);
		}

		// Invoke target slot (method)
		template<typename ClassType, typename FunctionPtrType>
		static ReType invoke(const Byte* const data, Args&&... args)
		{
			return
				(reinterpret_cast<const TargetSlot<ClassType, FunctionPtrType>*>
				(data)->mClassInstance->*
					reinterpret_cast<const TargetSlot<ClassType, FunctionPtrType>*>
					(data)->mFunctionPtr)(::std::forward<Args>(args)...);
		}

		// Invoke target slot (functor)
		template<typename ClassType, ::std::nullptr_t>
		static ReType invoke(const Byte* const data, Args&&... args)
		{
			return
				(*reinterpret_cast<const TargetSlot<ClassType, ::std::nullptr_t>*>
				(data)->mClassInstance)(::std::forward<Args>(args)...);
		}

	public:
		// Construct Slot (static method / free function)
		explicit Slot(ReType(*function)(Args&&...)) noexcept
			: mTarget{}
			, mInvoker{ nullptr }
		{
			using FunctionPtrType = decltype(function);
			auto Storage = reinterpret_cast<TargetSlot<::std::nullptr_t, FunctionPtrType>*>(&mTarget[0]);
			Storage->mFunctionPtr = function;
			Storage->mClassInstance = nullptr;
			mInvoker = &Slot::invoke<nullptr, FunctionPtrType>;
		}

		// Construct Slot (method)
		template<typename ClassType, typename FunctionPtrType>
		Slot(ClassType* object, FunctionPtrType method) noexcept
			: mTarget{}
			, mInvoker{ nullptr }
		{
			auto Storage = reinterpret_cast<TargetSlot<ClassType, FunctionPtrType>*>(&mTarget[0]);
			Storage->mFunctionPtr = method;
			Storage->mClassInstance = object;
			mInvoker = &Slot::invoke<ClassType, FunctionPtrType>;
		}

		// Construct Slot (functor)
		template<typename ClassType>
		explicit Slot(ClassType* functor) noexcept
			: mTarget{}
			, mInvoker{ nullptr }
		{
			auto Storage = reinterpret_cast<TargetSlot<ClassType, ::std::nullptr_t>*>(&mTarget[0]);
			Storage->mFunctionPtr = nullptr;
			Storage->mClassInstance = functor;
			mInvoker = &Slot::invoke<ClassType, nullptr>;
		}

		// Deleted constructor (null pointer)
		Slot(::std::nullptr_t) noexcept = delete;

		// Copy-construct Slot
		Slot(const Slot& other) noexcept
		{
			::std::copy(
				::std::cbegin(other.mTarget)
				, ::std::cend(other.mTarget)
				, ::std::begin(mTarget)
			);

			mInvoker = other.mInvoker;
		}

		// Destroy Slot
		~Slot() noexcept = default;

		// Copy-assign Slot
		Slot& operator=(const Slot& other) noexcept
		{
			if (this != &other)
			{
				::std::copy(
					::std::cbegin(other.mTarget)
					, ::std::cend(other.mTarget)
					, ::std::begin(mTarget)
				);

				mInvoker = other.mInvoker;
			}
			return *this;
		}

		// Invoke target slot
		ReType operator()(Args&&... args) const
		{
			// return (*mInvoker)(&mTarget[0], ::std::forward<Args>(args)...);
			return  ::std::invoke(*mInvoker, &mTarget[0], ::std::forward<Args>(args)...);
		}

		// Compare slot_functors (equal)
		bool operator==(const Slot& other) const noexcept
		{
			for (size_type index = 0; index < target_size; ++index)
			{
				if (mTarget[index] == other.mTarget[index])
				{
					return true;
				}
			}
			return false;
		}

		// Compare slot_functors (not equal)
		bool operator!=(const Slot& other) const noexcept
		{
			for (size_type index = 0; index < target_size; ++index)
			{
				if (mTarget[index] != other.mTarget[index])
				{
					return true;
				}
			}
			return false;
		}
	};

	class Connection final
	{
	public:
		// Construct Connection
		Connection(const Slot& slot, const TrackPtr& t_ptr, bool trackable) noexcept
			: m_slot{ slot }
			, m_track_ptr{ t_ptr }
			, mp_next{ nullptr }
			, mp_deleted{ nullptr }
			, m_trackable{ trackable }
		{}

		// Deleted copy-constructor
		Connection(const Connection&) noexcept = delete;

		// Destroy Connection
		~Connection() noexcept
		{}

		// Deleted copy-assignment operator
		Connection& operator=(const Connection&)noexcept = delete;

	public:
		Slot	m_slot;
		TrackPtr  m_track_ptr;
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
	bool connect(const Slot & slot,
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
	bool disconnect(const Slot & slot) noexcept
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
	bool connected(const Slot & slot) const noexcept
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
	void activate(Args&& ... args)
	{
		Connection* current = mp_first_slot.load();

		while (current)
		{
			if (!current->m_trackable)
			{
				current->m_slot(std::forward<Args>(args)...);
				current = current->mp_next.load();
			}
			else
			{
				auto ptr = current->m_track_ptr.lock();

				if (ptr)
				{
					current->m_slot(std::forward<Args>(args)...);
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
	bool connect(ReType(*function)(Args...))
	{
		const auto writer{ write_access() };
		return connect(Slot(function), TrackPtr(), false);
	}

	// Connect Signal to slot (method)
	// May throw exception if memory allocation fails
	template<typename ClassType, typename FunctionPtrType>
	bool connect(ClassType* object, FunctionPtrType method)
	{
		auto writer = write_access();
		return connect(Slot(object, method), TrackPtr(), false);
	}

	// Connect Signal to traceable slot (method)
	// May throw exception if memory allocation fails
	template<typename ClassType, typename FunctionPtrType>
	bool connect(::std::shared_ptr<ClassType> object, FunctionPtrType method)
	{
		auto writer = write_access();
		return connect(Slot(object.get(), method), TrackPtr(object), true);
	}

	// Connect Signal to slot (functor)
	// May throw exception if memory allocation fails
	template<typename ClassType>
	bool connect(ClassType* functor)
	{
		auto writer = write_access();
		return connect(Slot(functor), TrackPtr(), false);
	}

	// Connect Signal to traceable slot (functor)
	// May throw exception if memory allocation fails
	template<typename ClassType>
	bool connect(::std::shared_ptr<ClassType> functor)
	{
		auto writer = write_access();
		return connect(Slot(functor.get()), TrackPtr(functor), true);
	}

	// Disconnect Signal from slot (static method / free function)
	bool disconnect(ReType(*function)(Args...)) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot(function));
	}

	// Disconnect Signal from slot (method)
	template<typename ClassType, typename FunctionPtrType>
	bool disconnect(ClassType* object, FunctionPtrType method) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot(object, method));
	}

	// Disconnect Signal from traceable slot (method)
	template<typename ClassType, typename FunctionPtrType>
	bool disconnect(::std::shared_ptr<ClassType> object, FunctionPtrType method) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot(object.get(), method));
	}

	// Disconnect Signal from slot (functor)
	template<typename ClassType>
	bool disconnect(ClassType* functor) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot(functor));
	}

	// Disconnect Signal from track-able slot (functor)
	template<typename ClassType>
	bool disconnect(::std::shared_ptr<ClassType> functor) noexcept
	{
		auto writer = write_access();
		return disconnect(Slot(functor.get()));
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
		return connected(Slot(function));
	}

	// Check whether slot is connected (method)
	template<typename ClassType, typename FunctionPtrType>
	bool connected(ClassType* object, FunctionPtrType method) const noexcept
	{
		auto reader = read_access();
		return connected(Slot(object, method));
	}

	// Check whether traceable slot is connected (method)
	template<typename ClassType, typename FunctionPtrType>
	bool connected(::std::shared_ptr<ClassType> object, FunctionPtrType method) const noexcept
	{
		auto reader = read_access();
		return connected(Slot(object.get(), method));
	}

	// Check whether slot is connected (functor)
	template<typename ClassType>
	bool connected(ClassType* functor) const noexcept
	{
		auto reader = read_access();
		return connected(Slot(functor));
	}

	// Check whether traceable slot is connected (functor)
	template<typename ClassType>
	bool connected(::std::shared_ptr<ClassType> functor) const noexcept
	{
		auto reader = read_access();
		return connected(Slot(functor.get()));
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

JEJO_END

#endif // JEJO_SignalS_T_HPP

/*****************************************************************************/