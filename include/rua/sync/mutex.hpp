#ifndef _RUA_SYNC_MUTEX_HPP
#define _RUA_SYNC_MUTEX_HPP

#include "lockfree_list.hpp"

#include "../chrono/tick.hpp"
#include "../sched/scheduler.hpp"
#include "../types/util.hpp"

#include <atomic>
#include <cassert>

namespace rua {

class mutex {
public:
	constexpr mutex() : _locked(false), _waiters() {}

	mutex(const mutex &) = delete;

	mutex &operator=(const mutex &) = delete;

	bool try_lock(ms timeout = 0) {
		if (!_locked.exchange(true)) {
			return true;
		}
		if (!timeout) {
			return false;
		}

		auto sch = this_scheduler();
		auto wkr = sch->get_waker();

		if (!_waiters.emplace_if(
				[&]() -> bool { return _locked.exchange(true); }, wkr)) {
			return true;
		}

		if (timeout == duration_max()) {
			for (;;) {
				if (sch->sleep(timeout, true) &&
					!_waiters.emplace_if(
						[&]() -> bool { return _locked.exchange(true); },
						wkr)) {
					return true;
				}
			}
		}

		for (;;) {
			auto t = tick();
			auto r = sch->sleep(timeout, true);
			timeout -= tick() - t;
			if (timeout <= 0) {
				return !_locked.exchange(true);
			}
			if (r &&
				!_waiters.emplace_if(
					[&]() -> bool { return _locked.exchange(true); }, wkr)) {
				return true;
			}
		}
		return false;
	}

	void lock() {
		try_lock(duration_max());
	}

	void unlock() {
#ifdef NDEBUG
		_locked.store(false);
#else
		assert(_locked.exchange(false));
#endif
		auto waiter_opt = _waiters.try_pop();
		if (!waiter_opt) {
			return;
		}
		waiter_opt.value()->wake();
	}

private:
	std::atomic<bool> _locked;
	lockfree_list<waker_i> _waiters;
};

} // namespace rua

#endif
