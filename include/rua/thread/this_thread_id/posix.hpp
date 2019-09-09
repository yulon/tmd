#ifndef _RUA_THREAD_THIS_THREAD_ID_POSIX_HPP
#define _RUA_THREAD_THIS_THREAD_ID_POSIX_HPP

#include "../thread_id/posix.hpp"

#include "../../macros.hpp"

#include <pthread.h>

#include <functional>

namespace rua { namespace posix {

namespace _thread_this_thread_id {

RUA_FORCE_INLINE thread_id_t this_thread_id() {
	return pthread_self();
}

} // namespace _thread_this_thread_id

using namespace _thread_this_thread_id;

}} // namespace rua::posix

#endif