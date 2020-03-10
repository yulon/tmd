#ifndef _RUA_SYNC_CHAN_AUTO_HPP
#define _RUA_SYNC_CHAN_AUTO_HPP

#include "base.hpp"

#include "../../sched/util.hpp"

namespace rua {

template <typename T>
using chan = basic_chan<T, this_scheduler_getter>;

} // namespace rua

#endif
