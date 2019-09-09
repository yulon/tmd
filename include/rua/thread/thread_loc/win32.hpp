#ifndef _RUA_THREAD_THREAD_LOC_WIN32_HPP
#define _RUA_THREAD_THREAD_LOC_WIN32_HPP

#include "base.hpp"

#include "../../any_word.hpp"
#include "../../macros.hpp"
#include "../../sched/syswait/async.hpp"

#include <windows.h>

#include <cassert>
#include <utility>

namespace rua { namespace win32 {

class thread_loc_word {
public:
	thread_loc_word(void (*dtor)(any_word)) : _ix(TlsAlloc()), _dtor(dtor) {}

	~thread_loc_word() {
		if (!is_storable()) {
			return;
		}
		if (!TlsFree(_ix)) {
			return;
		}
		_ix = TLS_OUT_OF_INDEXES;
	}

	thread_loc_word(thread_loc_word &&src) : _ix(src._ix) {
		if (src.is_storable()) {
			src._ix = TLS_OUT_OF_INDEXES;
		}
	}

	RUA_OVERLOAD_ASSIGNMENT_R(thread_loc_word)

	bool is_storable() const {
		return _ix != TLS_OUT_OF_INDEXES;
	}

	void set(any_word value) {
		_get() = value;
	}

	any_word get() const {
		return _get();
	}

private:
	DWORD _ix;
	void (*_dtor)(any_word);

	any_word &_get() const {
		auto val_ptr = TlsGetValue(_ix);
		if (!val_ptr) {
			auto p = new any_word(0);
			TlsSetValue(_ix, p);
			auto h = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());
			auto dtor = _dtor;
			syswait(h, [=]() {
				dtor(*p);
				delete p;
				CloseHandle(h);
			});
			val_ptr = p;
		}
		return *reinterpret_cast<any_word *>(val_ptr);
	}
};

template <typename T>
using thread_loc = basic_thread_loc<T, thread_loc_word>;

}} // namespace rua::win32

#endif