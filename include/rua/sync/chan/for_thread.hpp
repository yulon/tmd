#ifndef _RUA_SYNC_CHAN_FOR_THREAD_HPP
#define _RUA_SYNC_CHAN_FOR_THREAD_HPP

#include "basic.hpp"

#include "../../sched/util.hpp"
#include "../../thread/scheduler.hpp"

namespace rua {

template <typename T>
using thread_chan = basic_chan<T, thread_scheduler_getter>;

} // namespace rua

#endif
