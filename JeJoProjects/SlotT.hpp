/******************************************************************************
 * Unified function pointer wrapper. Instances of Slot
 * an store, copy, and invoke any callable target (slot).
 *
 * @Authur :  JeJo
 * @Date   :  June - 2021
 * @license: free to use and distribute(no further support as well)
 *
 * @todo: explanation and limitations of the class
 *****************************************************************************/

#ifndef JEJO_SLOT_T_HPP
#define JEJO_SLOT_T_HPP

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

// Macros for dynamic memory allocation
#define NEW_MEMORY(arg) ::operator new(arg)
#define DELETE_MEMORY(arg) ::operator delete(arg)

// own JeJo-lib headers
namespace JeJo {


	// Synchronization stage. Specifies current access stage.
	enum class SyncStage : char { SyncStage_1 = 1, SyncStage_2 = 2 };

	using Byte = unsigned char;
	using size_type = ::std::size_t;
	using AtomicBoolType = ::std::atomic<bool>;
	using CounterType = ::std::atomic<unsigned short>;
	using AccessStage = ::std::atomic<SyncStage>;
	using TrackPtr = ::std::weak_ptr<void>;




	template<typename ResT, typename ... ArgTs> class Slot;

	// TEMPLATE CLASS Slot
	template<typename ReType, typename... Args> class Slot<ReType(Args...)> final
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
		// template<typename ReType, typename... Args>
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
		Slot(ReType(*function)(Args&&...)) noexcept
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



	template<typename ResT, typename ... ArgTs> class Connection;


	// TEMPLATE CLASS Slot
	template<typename ReType, typename... Args> class Connection<ReType(Args...)> final
	{
	public:
		Slot<ReType(Args...)> mSlot;
		TrackPtr mTrackPtr{ nullptr };
		::std::atomic<Connection<ReType(Args...)>*> mNextPtr{ nullptr };
		Connection* mDeletedPtr{ nullptr };
		const bool mTrackable{ false };

	public:
		// Construct Connection
		explicit Connection(const Slot<ReType(Args...)>& slot, const TrackPtr& trackPtr, bool trackable) noexcept
			: mSlot{ slot }
			, mTrackPtr{ trackPtr }
			, mNextPtr{ nullptr }
			, mDeletedPtr{ nullptr }
			, mTrackable{ trackable }
		{}

		// Deleted copy-constructor
		Connection(const Connection&) noexcept = delete;

		// Deleted copy-assignment operator
		Connection& operator=(const Connection&) noexcept = delete;

		// Destroy Connection
		~Connection() noexcept = default;
	};

}

#endif // JEJO_SLOT_T_HPP

/*****************************************************************************/