#ifndef mutex_h__
#define mutex_h__

#include <ogc/mutex.h>

/*!
 * hmmm it seems that devkitpro doesn't have all the c++11 features enabled. It seems to miss at least
 * std::mutex and std::thread.
 */
class mutex
{
public:
	mutex() noexcept
	{
		LWP_MutexInit(&mutex_, false);
	}

	~mutex() noexcept
	{
		LWP_MutexDestroy(mutex_);
	}

	mutex(const mutex&) = delete;
	mutex& operator=(const mutex&) = delete;

	void lock()
	{
		LWP_MutexLock(mutex_);
	}

	bool try_lock()
	{
		//! LWP_MutexTryLock returns negative on failure, return 0 if first lock and returns 1 if lock
		return !(LWP_MutexTryLock(mutex_) < 0);
	}

	void unlock()
	{
		LWP_MutexUnlock(mutex_);
	}
private:
	mutex_t mutex_;
};

struct defer_lock_t {};
struct try_to_lock_t {};
struct adopt_lock_t {};

constexpr defer_lock_t  defer_lock{};
constexpr try_to_lock_t try_to_lock{};
constexpr adopt_lock_t  adopt_lock{};

template <class Mutex>
class lock_guard
{
public:
	typedef Mutex mutex_type;
private:
	mutex_type& __m;
public:

	explicit lock_guard(mutex_type& m)
		: __m(m) {__m.lock();}

	lock_guard(mutex_type& m, adopt_lock_t)
		: __m(m) {}

	~lock_guard() {__m.unlock();}

	lock_guard(lock_guard const&) = delete;
	lock_guard& operator=(lock_guard const&) = delete;
};

class LockMutex
{
	mutex_t &m_mutex;
public:
	LockMutex(mutex_t &m) : m_mutex(m) { LWP_MutexLock(m_mutex); }
	~LockMutex(void) { LWP_MutexUnlock(m_mutex); }

	LockMutex(const LockMutex&) = delete;
	LockMutex& operator=(const LockMutex&) = delete;
};


#endif // mutex_h__
