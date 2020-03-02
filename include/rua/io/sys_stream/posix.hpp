#ifndef _RUA_IO_SYS_STREAM_POSIX_HPP
#define _RUA_IO_SYS_STREAM_POSIX_HPP

#include "../../macros.hpp"
#include "../abstract.hpp"

#include <unistd.h>

#include <cassert>

namespace rua { namespace posix {

class sys_stream : public read_write_closer {
public:
	using native_handle_t = int;

	constexpr sys_stream(native_handle_t fd = -1, bool need_close = true) :
		_fd(fd),
		_nc(need_close) {}

	template <
		typename NullPtr,
		typename = enable_if_t<std::is_same<NullPtr, std::nullptr_t>::value>>
	constexpr sys_stream(NullPtr) : sys_stream(-1, false) {}

	sys_stream(const sys_stream &src) {
		if (!src._fd) {
			_fd = -1;
			return;
		}
		_fd = ::dup(src._fd);
		assert(_fd >= 0);
		_nc = true;
	}

	sys_stream(sys_stream &&src) : sys_stream(src._fd, src._nc) {
		src.detach();
	}

	RUA_OVERLOAD_ASSIGNMENT(sys_stream)

	virtual ~sys_stream() {
		close();
	}

	native_handle_t &native_handle() {
		return _fd;
	}

	native_handle_t native_handle() const {
		return _fd;
	}

	explicit operator bool() const {
		return _fd >= 0;
	}

	virtual ptrdiff_t read(bytes_ref p) {
		return static_cast<ptrdiff_t>(::read(_fd, p.data(), p.size()));
	}

	virtual ptrdiff_t write(bytes_view p) {
		return static_cast<ptrdiff_t>(::write(_fd, p.data(), p.size()));
	}

	virtual void close() {
		if (_fd < 0) {
			return;
		}
		if (_nc) {
			::close(_fd);
		}
		_fd = -1;
	}

	void detach() {
		_nc = false;
	}

private:
	native_handle_t _fd;
	bool _nc;
};

}} // namespace rua::posix

#endif