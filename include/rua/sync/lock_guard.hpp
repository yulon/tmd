#ifndef _RUA_SYNC_LOCK_GUARD_HPP
#define _RUA_SYNC_LOCK_GUARD_HPP

#include "../chrono/duration.hpp"
#include "../macros.hpp"

namespace rua {

template <typename Lock>
class lock_guard {
public:
	explicit lock_guard(Lock &lck, duration try_lock_timeout = duration_max()) :
		_lck(&lck) {
		_lck->try_lock(try_lock_timeout);
	}

	~lock_guard() {
		unlock();
	}

	lock_guard(lock_guard &&src) : _lck(src._lck) {
		if (src._lck) {
			src._lck = nullptr;
		}
	}

	RUA_OVERLOAD_ASSIGNMENT_R(lock_guard)

	explicit operator bool() const {
		return _lck;
	}

	void unlock() {
		if (!_lck) {
			return;
		}
		_lck->unlock();
		_lck = nullptr;
	}

private:
	Lock *_lck;
};

template <typename Lock>
inline lock_guard<Lock>
make_lock_guard(Lock &lck, duration try_lock_timeout = duration_max()) {
	return lock_guard<Lock>(lck, try_lock_timeout);
}

} // namespace rua

#endif
