/******************************************************************************
 * SlimLock - Synchronization primitive that protects shared data from being
 * simultaneously modified by multiple threads. Optimized for speed
 * and occupies very little memory. Meets Lockable requirements.
 * 
 * AutoLock - ClassType AutoLock is a SlimLock ownership wrapper that provides a
 * convenient RAII-style mechanism for automatic locking / unlocking.
 * 
 * ReadGuard - ClassType ReadGuard is a stage counter ownership wrapper. Provides
 * a convenient RAII-style mechanism for automatic increm / decrem.
 *
 * @Authur :  JeJo
 * @Date   :  June - 2021
 * @license: free to use and distribute(no further support as well)
 *
 * @todo: explanation and limitations of the class
 *****************************************************************************/

#ifndef JEJO_LOCK_CLASSES_T_HPP
#define JEJO_LOCK_CLASSES_T_HPP

 // C++ headers
#include <atomic>       // std::atomic_flag, std::memory_order_xxxx
#include <thread>       // std::this_thread::yield
#include <utility>      // std::exchange


// own JeJo-lib headers
//#include "SlotT.hpp"

namespace JeJo::internal
{
	class SlimLock final
	{
	private:
		using lock_type = std::atomic_flag;

		lock_type m_lock = ATOMIC_FLAG_INIT;

	public:
		// Construct SlimLock
		SlimLock() noexcept = default;

		// Deleted copy-constructor
		SlimLock(const SlimLock&)noexcept = delete;

		// Deleted copy-assignment operator
		SlimLock& operator=(const SlimLock&)noexcept = delete;

		// Destroy SlimLock
		~SlimLock() noexcept = default;

		// Lock SlimLock. If another thread has already locked this
		// SlimLock, blocks execution until the lock is acquired.
		void lock() noexcept
		{
			while (m_lock.test_and_set(std::memory_order_acquire))
			{
				std::this_thread::yield();
			}
		}

		// Unlock SlimLock
		void unlock() noexcept
		{
			m_lock.clear(std::memory_order_release);
		}

		// Try to lock SlimLock. Returns immediately. On successful
		// lock acquisition returns true, otherwise returns false.
		bool try_lock() noexcept
		{
			return m_lock.test_and_set(std::memory_order_acquire) ? false : true;
		}

	};

	class AutoLock final
	{
	private:
		SlimLock* mLockPtr{ nullptr };

	public:
		// Construct AutoLock
		explicit AutoLock(SlimLock& s_lock) noexcept
			: mLockPtr{ &s_lock }
		{
			mLockPtr->lock();
		}

		// Move-construct AutoLock
		AutoLock(AutoLock&& other) noexcept
			: mLockPtr{ std::exchange(other.mLockPtr, nullptr) }
		{}

		// Move-assign AutoLock
		AutoLock& operator=(AutoLock&& other) noexcept
		{
			mLockPtr->unlock();
			mLockPtr = std::exchange(other.mLockPtr, nullptr);
			return *this;
		}

		// Destroy AutoLock
		~AutoLock() noexcept
		{
			if (mLockPtr)
			{
				mLockPtr->unlock();
			}
		}
	};

	class ReadGuard final
	{

	private:
		CounterType* mAtomicCounter{ nullptr };
	public:

		// Construct ReadGuard
		explicit ReadGuard(CounterType& counter) noexcept
			: mAtomicCounter{ &counter }
		{
			++(*mAtomicCounter);
		}

		// Move-construct ReadGuard
		ReadGuard(ReadGuard&& other) noexcept
			: mAtomicCounter{ std::exchange(other.mAtomicCounter, nullptr) }
		{}

		// Move-assign ReadGuard
		ReadGuard& operator=(ReadGuard&& other) noexcept
		{
			--(*mAtomicCounter);
			mAtomicCounter = std::exchange(other.mAtomicCounter, nullptr);
			return *this;
		}

		// Destroy ReadGuard
		~ReadGuard() noexcept
		{
			if (mAtomicCounter)
			{
				--(*mAtomicCounter);
			}
		}
	};
}

#endif // JEJO_LOCK_CLASSES_T_HPP

/*****************************************************************************/