#ifndef _RUA_PRINTER_HPP
#define _RUA_PRINTER_HPP

#include "io.hpp"
#include "string.hpp"
#include "types/util.hpp"

#include <string>

namespace rua {

class printer {
public:
	printer() {}

	printer(std::nullptr_t) : printer() {}

	explicit printer(writer_i w, const char *eol = eol::lf) :
		_w(std::move(w)), _eol(eol) {}

	~printer() {
		if (!_w) {
			return;
		}
		_w.reset();
	}

	template <typename... Args>
	void print(Args &&... args) {
		if (!_w) {
			return;
		}
		str_join(
			_buf,
			std::initializer_list<string_view>{to_temp_string(args)...},
			" ",
			str_join_multi_line);
		_w->write_all(_buf);
		_buf.resize(0);
	}

	template <typename... Args>
	void println(Args &&... args) {
		print(std::forward<Args>(args)..., _eol);
	}

	void reset(writer_i w = nullptr) {
		_w = w;
	}

	void reset(writer_i w, const char *eol) {
		_w = w;
		_eol = eol;
	}

private:
	writer_i _w;
	const char *_eol;
	std::string _buf;
};

} // namespace rua

#endif
