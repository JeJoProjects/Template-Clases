#ifndef SIGNALS_LITE_T_HPP
#define SIGNALS_LITE_T_HPP

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

	using Byte = unsigned char;
	using size_type = unsigned short;
	using TrackPtr = ::std::weak_ptr<void>;

	static const size_type null_index = ((size_type)-1);

	class SlotFunctor final
	{
		// Structure to store target pointers
		template<typename Class, typename Signature>
		struct TargetSlot
		{
			using Signature = Signature;
			using SlotInstance = Class * ;
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
		using InvokerType = ResT(*)(const Byte * const, ArgTs&...);

		// Invoke target slot (static method / free function)
		template<::std::nullptr_t, typename Signature>
		static ResT invoke(const Byte * const data, ArgTs& ... args)
		{
			return
				(*reinterpret_cast<const TargetSlot<::std::nullptr_t, Signature>*>
				(data)->mp_function)(args...);
		}

		// Invoke target slot (method)
		template<typename Class, typename Signature>
		static ResT invoke(const Byte * const data, ArgTs& ... args)
		{
			return
				(reinterpret_cast<const TargetSlot<Class, Signature>*>
				(data)->mp_instance->*
					reinterpret_cast<const TargetSlot<Class, Signature>*>
					(data)->mp_function)(args...);
		}

		// Invoke target slot (functor)
		template<typename Class, ::std::nullptr_t>
		static ResT invoke(const Byte * const data, ArgTs& ... args)
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
			auto storage = reinterpret_cast<TargetSlot
				<::std::nullptr_t, Signature>*>(&m_target[0]);
			storage->mp_function = function;
			storage->mp_instance = nullptr;
			mp_invoker = &SlotFunctor::invoke<nullptr, Signature>;
		}

		// Construct SlotFunctor (method)
		template<typename Class, typename Signature>
		SlotFunctor(Class * object, Signature method) noexcept
			: m_target{}, mp_invoker{ nullptr }
		{
			auto storage = reinterpret_cast<TargetSlot
				<Class, Signature>*>(&m_target[0]);
			storage->mp_function = method;
			storage->mp_instance = object;
			mp_invoker = &SlotFunctor::invoke<Class, Signature>;
		}

		// Construct SlotFunctor (functor)
		template<typename Class>
		explicit SlotFunctor(Class * functor) noexcept
			: m_target{}, mp_invoker{ nullptr }
		{
			auto storage = reinterpret_cast<TargetSlot
				<Class, ::std::nullptr_t>*>(&m_target[0]);
			storage->mp_function = nullptr;
			storage->mp_instance = functor;
			mp_invoker = &SlotFunctor::invoke<Class, nullptr>;
		}

		// Deleted constructor (null pointer)
		SlotFunctor(::std::nullptr_t)noexcept = delete;

		// Copy-construct SlotFunctor
		SlotFunctor(const SlotFunctor & other) noexcept
		{
			::std::copy(other.m_target,
				other.m_target + target_size, m_target);
			mp_invoker = other.mp_invoker;
		}

		// Destroy SlotFunctor
		~SlotFunctor() noexcept
		{}

		// Copy-assign SlotFunctor
		SlotFunctor & operator=(const SlotFunctor & other) noexcept
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
		bool operator==(const SlotFunctor & other) const noexcept
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
		bool operator!=(const SlotFunctor & other) const noexcept
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
		Connection(const SlotFunctor & slot,
			const TrackPtr & t_ptr,
			bool trackable) noexcept
			: m_slot(slot),
			m_track_ptr(t_ptr),
			m_next(null_index),
			m_trackable(trackable)
		{}

		// Copy-construct Connection
		Connection(const Connection & other) noexcept
			: m_slot(other.m_slot),
			m_track_ptr(other.m_track_ptr),
			m_next(other.m_next),
			m_trackable(other.m_trackable)
		{}

		// Destroy Connection
		~Connection() noexcept
		{}

		// Copy-assign Connection
		Connection & operator=(const Connection & other) noexcept
		{
			if (this != &other)
			{
				m_slot = other.m_slot;
				m_track_ptr = other.m_track_ptr;
				m_next = other.m_next;
				m_trackable = other.m_trackable;
			}
			return *this;
		}

	public:

		SlotFunctor	m_slot;
		TrackPtr		m_track_ptr;
		size_type		m_next;
		const bool		m_trackable;

	};

	// Initialize storage
	void init_storage(Connection * address, size_type capacity) noexcept
	{
		for (size_type index = 1; index < capacity; ++index)
		{
			(*reinterpret_cast<size_type*>(address)) = index;
			++address;
		}
		(*reinterpret_cast<size_type*>(address)) = null_index;
	}

	// Expand memory by allocating new memory block
	// May throw exception if memory allocation fails
	void expand_storage()
	{
		size_type new_capacity = m_capacity * 2;
		Connection * new_block = reinterpret_cast<Connection*>
			(NEW_MEMORY(new_capacity * sizeof(Connection)));

		init_storage(new_block, new_capacity);
		m_store = m_capacity;

		for (size_type index = 0; index < m_capacity; ++index)
		{
			::new (new_block + index) Connection(*(mp_block + index));
			(mp_block + index)->~Connection();
		}

		DELETE_MEMORY(mp_block);
		m_capacity = new_capacity;
		mp_block = new_block;
	}

	// Allocate memory from storage
	// May throw exception if memory allocation fails
	size_type allocate()
	{
		if (m_store == null_index)
		{
			expand_storage(); // May throw
		}
		size_type current = m_store;
		m_store = *(reinterpret_cast<size_type*>(mp_block + m_store));
		return current;
	}

	// Deallocate previously allocated memory
	void deallocate(size_type index) noexcept
	{
		(*reinterpret_cast<size_type*>(mp_block + index)) = m_store;
		m_store = index;
	}

	// Connect new slot to the Signal
	// May throw exception if memory allocation fails
	bool connect(const SlotFunctor & slot,
		const TrackPtr & t_ptr,
		bool trackable)
	{
		if (m_first_slot != null_index)
		{
			size_type current = m_first_slot;
			size_type previous = null_index;

			while (current != null_index)
			{
				if (mp_block[current].m_slot == slot)
				{
					return false;
				}
				else
				{
					previous = current;
					current = mp_block[current].m_next;
				}
			}

			size_type index = allocate();
			::new (&mp_block[index]) Connection(slot, t_ptr, trackable);
			mp_block[previous].m_next = index;
			return true;
		}
		else
		{
			size_type index = allocate();
			::new (&mp_block[index]) Connection(slot, t_ptr, trackable);
			m_first_slot = index;
			return true;
		}
	}

	// Disconnect slot from the Signal
	bool disconnect(const SlotFunctor & slot) noexcept
	{
		if (m_first_slot != null_index)
		{
			size_type current = m_first_slot;

			if (mp_block[current].m_slot == slot)
			{
				m_first_slot = mp_block[current].m_next;
				mp_block[current].~Connection();
				deallocate(current);
				return true;
			}
			else
			{
				size_type previous = current;
				current = mp_block[current].m_next;

				while (current != null_index)
				{
					if (mp_block[current].m_slot == slot)
					{
						mp_block[previous].m_next = mp_block[current].m_next;
						mp_block[current].~Connection();
						deallocate(current);
						return true;
					}
					else
					{
						previous = current;
						current = mp_block[current].m_next;
					}
				}
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	// Check whether slot is connected to the Signal
	bool connected(const SlotFunctor & slot) const noexcept
	{
		size_type current = m_first_slot;

		while (current != null_index)
		{
			if (mp_block[current].m_slot == slot)
			{
				return true;
			}
			else
			{
				current = mp_block[current].m_next;
			}
		}

		return false;
	}

	// Activate Signal
	// May throw exception if some slot does
	void activate(ArgTs ... args)
	{
		if (!m_blocked)
		{
			size_type current = m_first_slot;

			while (current != null_index)
			{
				if (!mp_block[current].m_trackable)
				{
					mp_block[current].m_slot(args...);
					current = mp_block[current].m_next;
				}
				else
				{
					auto ptr = mp_block[current].m_track_ptr.lock();

					if (ptr)
					{
						mp_block[current].m_slot(args...);
						current = mp_block[current].m_next;
					}
					else
					{
						size_type to_delete = current;
						current = mp_block[current].m_next;
						disconnect(mp_block[to_delete].m_slot);
					}
				}
			}
		}
	}

public:

	// Construct Signal with provided / default capacity
	// May throw exception if memory allocation fails
	explicit Signal(size_type capacity = 5)
		: mp_block(nullptr),
		m_first_slot(null_index),
		m_capacity(capacity >= 1 ? capacity : 1),
		m_store(0),
		m_blocked(false)
	{
		mp_block = reinterpret_cast<Connection*>
			(NEW_MEMORY(m_capacity * sizeof(Connection)));
		init_storage(mp_block, m_capacity);
	}

	// Deleted copy-constructor
	Signal(const Signal &)noexcept = delete;

	// Destroy Signal
	~Signal() noexcept
	{
		disconnect_all();
		DELETE_MEMORY(mp_block);
	}

	// Deleted copy-assignment operator
	Signal & operator=(const Signal &)noexcept = delete;

	// Connect Signal to slot (function)
	// May throw exception if memory allocation fails
	bool connect(ResT(*function)(ArgTs...))
	{
		return connect(SlotFunctor(function), TrackPtr(), false);
	}

	// Connect Signal to slot (method)
	// May throw exception if memory allocation fails
	template<typename Class, typename Signature>
	bool connect(Class * object, Signature method)
	{
		return connect(SlotFunctor(object, method), TrackPtr(), false);
	}

	// Connect Signal to trackable slot (method)
	// May throw exception if memory allocation fails
	template<typename Class, typename Signature>
	bool connect(::std::shared_ptr<Class> object, Signature method)
	{
		return connect(SlotFunctor(object.get(), method), TrackPtr(object), true);
	}

	// Connect Signal to slot (functor)
	// May throw exception if memory allocation fails
	template<typename Class>
	bool connect(Class * functor)
	{
		return connect(SlotFunctor(functor), TrackPtr(), false);
	}

	// Connect Signal to trackable slot (functor)
	// May throw exception if memory allocation fails
	template<typename Class>
	bool connect(::std::shared_ptr<Class> functor)
	{
		return connect(SlotFunctor(functor.get()), TrackPtr(functor), true);
	}

	// Disconnect Signal from slot (function)
	bool disconnect(ResT(*function)(ArgTs...)) noexcept
	{
		return disconnect(SlotFunctor(function));
	}

	// Disconnect Signal from slot (method)
	template<typename Class, typename Signature>
	bool disconnect(Class * object, Signature method) noexcept
	{
		return disconnect(SlotFunctor(object, method));
	}

	// Disconnect Signal from trackable slot (method)
	template<typename Class, typename Signature>
	bool disconnect(::std::shared_ptr<Class> object, Signature method) noexcept
	{
		return disconnect(SlotFunctor(object.get(), method));
	}

	// Disconnect Signal from slot (functor)
	template<typename Class>
	bool disconnect(Class * functor) noexcept
	{
		return disconnect(SlotFunctor(functor));
	}

	// Disconnect Signal from trackable slot (functor)
	template<typename Class>
	bool disconnect(::std::shared_ptr<Class> functor) noexcept
	{
		return disconnect(SlotFunctor(functor.get()));
	}

	// Disconnect Signal from all slots
	void disconnect_all() noexcept
	{
		size_type to_delete = m_first_slot;

		while (to_delete != null_index)
		{
			size_type current = mp_block[to_delete].m_next;
			mp_block[to_delete].~Connection();
			deallocate(to_delete);
			to_delete = current;
		}

		m_first_slot = null_index;
	}

	// Check whether slot is connected (function)
	bool connected(ResT(*function)(ArgTs...)) const noexcept
	{
		return connected(SlotFunctor(function));
	}

	// Check whether slot is connected (method)
	template<typename Class, typename Signature>
	bool connected(Class * object, Signature method) const noexcept
	{
		return connected(SlotFunctor(object, method));
	}

	// Check whether trackable slot is connected (method)
	template<typename Class, typename Signature>
	bool connected(::std::shared_ptr<Class> object, Signature method) const noexcept
	{
		return connected(SlotFunctor(object.get(), method));
	}

	// Check whether slot is connected (functor)
	template<typename Class>
	bool connected(Class * functor) const noexcept
	{
		return connected(SlotFunctor(functor));
	}

	// Check whether trackable slot is connected (functor)
	template<typename Class>
	bool connected(::std::shared_ptr<Class> functor) const noexcept
	{
		return connected(SlotFunctor(functor.get()));
	}

	// Block Signal
	void block(bool block = true) noexcept
	{
		m_blocked = block;
	}

	// Check whether Signal is blocked
	bool blocked() const noexcept
	{
		return m_blocked;
	}

	// Emit Signal
	void emit(ArgTs ... args)
	{
		activate(args...);
	}

	// Emit Signal
	void operator()(ArgTs ... args)
	{
		activate(args...);
	}

	// Get number of connected slots
	size_type size() const noexcept
	{
		size_type current = m_first_slot;
		size_type size = 0;

		while (current != null_index)
		{
			++size;
			current = mp_block[current].m_next;
		}

		return size;
	}

	// Check whether list of connected slots is empty
	bool empty() const noexcept
	{
		return m_first_slot == null_index;
	}

private:

	Connection *	mp_block;
	size_type		m_first_slot;
	size_type		m_capacity;
	size_type		m_store;
	bool			m_blocked;

};

#endif // SIGNALS_LITE_T_HPP
