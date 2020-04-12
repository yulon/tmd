#ifndef _RUA_STRING_STR_JOIN_HPP
#define _RUA_STRING_STR_JOIN_HPP

#include "line.hpp"
#include "string_view.hpp"

#include "../macros.hpp"
#include "../types/traits.hpp"
#include "../types/util.hpp"

#include <string>

namespace rua {

struct str_join_options {
	bool is_ignore_empty = false;
	bool is_multi_line = false;
	size_t pre_reserved_size = 0;
};

template <
	typename StrList = std::initializer_list<string_view>,
	typename = enable_if_t<std::is_convertible<
		decltype(*std::declval<const StrList &>().begin()),
		string_view>::value>>
inline void str_join(
	std::string &buf,
	const StrList &strs,
	string_view sep = "",
	const str_join_options &opt = {}) {

	if (sep.empty()) {
		size_t len = 0;
		for (auto &str : strs) {
			len += str.size();
		}
		if (!len) {
			return;
		}
		buf.reserve(buf.size() + len + opt.pre_reserved_size);
		for (auto &str : strs) {
			buf += str;
		}
		return;
	}

	size_t len = 0;
	bool bf_is_eol = true;
	for (auto &str : strs) {
		if (!str.size() && (opt.is_ignore_empty)) {
			continue;
		}
		if (bf_is_eol) {
			bf_is_eol = false;
		} else {
			len += sep.size();
		}
		len += str.size();
		if (opt.is_multi_line && is_eol(str)) {
			bf_is_eol = true;
		}
	}

	if (!len) {
		return;
	}
	buf.reserve(buf.size() + len + opt.pre_reserved_size);

	bf_is_eol = true;
	for (auto &str : strs) {
		if (!str.size() && opt.is_ignore_empty) {
			continue;
		}
		if (bf_is_eol) {
			bf_is_eol = false;
		} else {
			buf += sep;
		}
		buf += str;
		if (opt.is_multi_line && is_eol(str)) {
			bf_is_eol = true;
		}
	}
	return;
}

template <
	typename StrList = std::initializer_list<string_view>,
	typename = enable_if_t<std::is_convertible<
		decltype(*std::declval<const StrList &>().begin()),
		string_view>::value>>
RUA_FORCE_INLINE std::string str_join(
	const StrList &strs,
	string_view sep = "",
	const str_join_options &opt = {}) {
	std::string r;
	str_join(r, strs, sep, opt);
	return r;
}

template <
	typename... Strs,
	typename = decltype(std::initializer_list<string_view>{
		std::declval<Strs>()...})>
RUA_FORCE_INLINE std::string str_join(Strs &&... strs) {
	return str_join({strs...});
}

} // namespace rua

#endif
