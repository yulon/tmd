#ifndef _RUA_SYNC_CHAN_BASIC_HPP
#define _RUA_SYNC_CHAN_BASIC_HPP

#include "../lf_forward_list.hpp"

#include "../../chrono/clock.hpp"
#include "../../macros.hpp"
#include "../../opt.hpp"
#include "../../sched/scheduler.hpp"

#include <cassert>
#include <queue>
#include <utility>

namespace rua {

template <typename T>
class chan {
public:
	constexpr chan() : _buf(), _waiters() {}

	chan(const chan &) = delete;

	chan &operator=(const chan &) = delete;

	rua::opt<T> try_pop() {
		return _buf.pop_back();
	}

	rua::opt<T> try_pop(ms timeout = 0);

	T pop() {
		return try_pop(duration_max()).value();
	}

	rua::opt<T> try_pop(scheduler_i sch, ms timeout = 0) {
		auto val_opt = _buf.pop_back();
		if (val_opt || !timeout) {
			return val_opt;
		}
		return _try_pop(sch, timeout);
	}

	T pop(scheduler_i sch) {
		return try_pop(std::move(sch), duration_max()).value();
	}

	template <typename... Args>
	bool emplace(Args &&... args) {
		_buf.emplace_front(std::forward<Args>(args)...);
		auto waiter_opt = _waiters.pop_back();
		if (!waiter_opt) {
			return false;
		}
		waiter_opt.value()->signal();
		return true;
	}

private:
	lf_forward_list<T> _buf;
	lf_forward_list<scheduler::signaler_i> _waiters;

	rua::opt<T> _try_pop(scheduler_i sch, ms timeout) {
		assert(timeout);

		rua::opt<T> val_opt;

		auto sig = sch->get_signaler();
		auto it = _waiters.emplace_front(sig);

		if (it.is_back()) {
			val_opt = _buf.pop_back();
			if (val_opt) {
				if (!_waiters.erase(it) && !_buf.is_empty()) {
					auto waiter_opt = _waiters.pop_back();
					if (waiter_opt && waiter_opt.value() != sig) {
						waiter_opt.value()->signal();
					}
				}
				return val_opt;
			}
		}

		if (timeout == duration_max()) {
			for (;;) {
				if (sch->wait(timeout)) {
					val_opt = _buf.pop_back();
					if (val_opt) {
						return val_opt;
					}
					it = _waiters.emplace_front(sig);
				}
			}
		}

		for (;;) {
			auto t = tick();
			auto r = sch->wait(timeout);
			val_opt = _buf.pop_back();
			if (val_opt || !r) {
				return val_opt;
			}
			timeout -= tick() - t;
			if (timeout <= 0) {
				return val_opt;
			}
			it = _waiters.emplace_front(sig);
		}
		return val_opt;
	}
};

template <typename T, typename R>
RUA_FORCE_INLINE chan<T> &operator>>(chan<T> &ch, R &receiver) {
	receiver = ch.pop();
	return ch;
}

template <typename T, typename R>
RUA_FORCE_INLINE chan<T> &operator<<(R &receiver, chan<T> &ch) {
	receiver = ch.pop();
	return ch;
}

template <typename T, typename V>
RUA_FORCE_INLINE chan<T> &operator<<(chan<T> &ch, V &&val) {
	ch.emplace(std::forward<V>(val));
	return ch;
}

template <typename T, typename V>
RUA_FORCE_INLINE chan<T> &operator>>(V &&val, chan<T> &ch) {
	ch.emplace(std::forward<V>(val));
	return ch;
}

} // namespace rua

#endif