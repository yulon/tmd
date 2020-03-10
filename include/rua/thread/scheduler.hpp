#ifndef _RUA_THREAD_SCHEDULER_HPP
#define _RUA_THREAD_SCHEDULER_HPP

#include "../macros.hpp"

#ifdef _WIN32

#include "scheduler/win32.hpp"

namespace rua {

using thread_scheduler = win32::thread_scheduler;
using thread_scheduler_getter = win32::thread_scheduler_getter;

} // namespace rua

#elif defined(RUA_DARWIN)

#include "scheduler/darwin.hpp"

namespace rua {

using thread_scheduler = darwin::thread_scheduler;
using thread_scheduler_getter = darwin::thread_scheduler_getter;

} // namespace rua

#elif defined(RUA_UNIX) || RUA_HAS_INC(<pthread.h>) || defined(_PTHREAD_H)

#include "scheduler/posix.hpp"

namespace rua {

using thread_scheduler = posix::thread_scheduler;
using thread_scheduler_getter = posix::thread_scheduler_getter;

} // namespace rua

#endif

#endif
