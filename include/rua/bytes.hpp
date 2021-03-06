#ifndef _RUA_BYTES_HPP
#define _RUA_BYTES_HPP

/*
	Q: Why don't I use std::byte?
	A: Because I suspect std::byte is used to make Microsoft feel sick.

	1. All operators are implemented by operator overloading,
	   but MSVC does not inline code in debug mode,
	   which causes programs in debug mode to run slowly.

	2. List initialization declarations are very redundant,
	   such as {std::byte{n}, ...} or {static_cast<std::byte>(n), ...},
	   but G++ can use {{n}, ...}.
*/

#include "bit.hpp"
#include "macros.hpp"
#include "optional.hpp"
#include "range.hpp"
#include "span.hpp"
#include "string/conv.hpp"
#include "string/len.hpp"
#include "string/view.hpp"
#include "types/traits.hpp"
#include "types/util.hpp"

#include <cassert>
#include <cstring>
#include <string>
#include <vector>

namespace rua {

class bytes_view;
class bytes_ref;
class bytes;
class bytes_pattern;

template <typename Bytes>
class basic_bytes_finder;

using const_bytes_finder = basic_bytes_finder<bytes_view>;
using bytes_finder = basic_bytes_finder<bytes_ref>;

using const_bytes_rfinder = reverse_iterator<const_bytes_finder>;
using bytes_rfinder = reverse_iterator<bytes_finder>;

template <typename Span>
class const_bytes_base : private disable_as_to_string {
public:
	generic_ptr data_generic() const {
		return _this()->data();
	}

	constexpr explicit operator bool() const {
		return _this()->size();
	}

	template <typename T>
	T get() const {
		return bit_get<T>(_this()->data());
	}

	template <typename T>
	T get(ptrdiff_t offset) const {
		return bit_get<T>(_this()->data(), offset);
	}

	template <typename T>
	T aligned_get(ptrdiff_t ix) const {
		return bit_aligned_get<T>(_this()->data(), ix);
	}

	template <typename T>
	const T &as() const {
		return bit_as<const T>(_this()->data());
	}

	template <typename T>
	const T &as(ptrdiff_t offset) const {
		return bit_as<const T>(_this()->data(), offset);
	}

	template <typename T>
	const T &aligned_as(ptrdiff_t ix) const {
		return bit_aligned_as<const T>(_this()->data(), ix);
	}

	const uchar &operator[](ptrdiff_t offset) const {
		return _this()->data()[offset];
	}

	inline bytes_view
	slice(ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const;

	inline bytes_view slice(ptrdiff_t begin_offset) const;

	inline bytes_view
	operator()(ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const;

	inline bytes_view operator()(ptrdiff_t begin_offset) const;

	template <typename... DestArgs>
	inline size_t copy_to(DestArgs &&... dest) const;

	template <typename RelPtr, size_t SlotSize = sizeof(RelPtr)>
	generic_ptr derel(ptrdiff_t pos = 0) const {
		return _this()->data() + pos + SlotSize + get<RelPtr>(pos);
	}

	inline bool eq(bytes_view) const;

	inline bool operator==(bytes_view) const;

	inline size_t index_of(const bytes_pattern &, size_t start_pos = 0) const;

	inline const_bytes_finder find(bytes_pattern, size_t start_pos = 0) const;

	inline size_t
	last_index_of(const bytes_pattern &, size_t start_pos = nullpos) const;

	inline const_bytes_rfinder
	rfind(bytes_pattern, size_t start_pos = nullpos) const;

protected:
	const_bytes_base() = default;

private:
	const Span *_this() const {
		return static_cast<const Span *>(this);
	}
};

template <typename Span>
class bytes_base : public const_bytes_base<Span> {
public:
	template <typename T>
	void set(const T &val) {
		return bit_set<T>(_this()->data(), val);
	}

	template <typename T>
	void set(const T &val, ptrdiff_t offset) {
		return bit_set<T>(_this()->data(), offset, val);
	}

	template <typename T>
	void aligned_set(const T &val, ptrdiff_t ix) {
		return bit_aligned_set<T>(_this()->data(), val, ix);
	}

	template <typename T>
	const T &as() const {
		return bit_as<const T>(_this()->data());
	}

	template <typename T>
	T &as() {
		return bit_as<T>(_this()->data());
	}

	template <typename T>
	const T &as(ptrdiff_t offset) const {
		return bit_as<const T>(_this()->data(), offset);
	}

	template <typename T>
	T &as(ptrdiff_t offset) {
		return bit_as<T>(_this()->data(), offset);
	}

	template <typename T>
	const T &aligned_as(ptrdiff_t ix) const {
		return bit_aligned_as<const T>(_this()->data(), ix);
	}

	template <typename T>
	T &aligned_as(ptrdiff_t ix) {
		return bit_aligned_as<T>(_this()->data(), ix);
	}

	const uchar &operator[](ptrdiff_t offset) const {
		return _this()->data()[offset];
	}

	uchar &operator[](ptrdiff_t offset) {
		return _this()->data()[offset];
	}

	inline bytes_view
	slice(ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const;

	inline bytes_view slice(ptrdiff_t begin_offset) const;

	inline bytes_ref
	slice(ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin);

	inline bytes_ref slice(ptrdiff_t begin_offset);

	inline bytes_view
	operator()(ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const;

	inline bytes_view operator()(ptrdiff_t begin_offset) const;

	inline bytes_ref
	operator()(ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin);

	inline bytes_ref operator()(ptrdiff_t begin_offset);

	template <typename... SrcArgs>
	inline size_t copy_from(SrcArgs &&... src) const;

	template <typename RelPtr, size_t SlotSize = sizeof(RelPtr)>
	RelPtr enrel(generic_ptr abs_ptr, ptrdiff_t pos = 0) {
		auto rel_ptr =
			static_cast<RelPtr>(abs_ptr - (_this()->data() + pos + SlotSize));
		this->template set<RelPtr>(pos, rel_ptr);
		return rel_ptr;
	}

	template <typename T = uchar>
	void reverse() {
		auto n = _this()->size() / sizeof(T);
		std::vector<T> r(n);
		auto p = reinterpret_cast<T *>(_this()->data());
		for (size_t i = 0; i < n; ++i) {
			r[i] = p[n - 1 - i];
		}
		for (size_t i = 0; i < n; ++i) {
			p[i] = r[n - 1 - i];
		}
	}

	inline bytes_finder find(bytes_pattern, size_t start_pos = 0);

	inline const_bytes_finder find(bytes_pattern, size_t start_pos = 0) const;

	inline bytes_rfinder rfind(bytes_pattern, size_t start_pos = nullpos);

	inline const_bytes_rfinder
	rfind(bytes_pattern, size_t start_pos = nullpos) const;

private:
	Span *_this() {
		return static_cast<Span *>(this);
	}

	const Span *_this() const {
		return static_cast<const Span *>(this);
	}
};

class bytes_view : public const_bytes_base<bytes_view> {
public:
	constexpr bytes_view(std::nullptr_t = nullptr) : _p(nullptr), _n(0) {}

	constexpr bytes_view(const uchar *ptr, size_t size) :
		_p(ptr), _n(ptr ? size : 0) {}

	RUA_CONSTEXPR_14 bytes_view(std::initializer_list<uchar> il) :
		bytes_view(il.begin(), il.size()) {}

	template <
		typename Span,
		typename SpanTraits = span_traits<Span &&>,
		typename = enable_if_t<
			!std::is_base_of<bytes_view, decay_t<Span>>::value &&
			std::is_same<typename SpanTraits::value_type, uchar>::value>>
	constexpr bytes_view(Span &&span) :
		bytes_view(
			span_data(std::forward<Span>(span)),
			span_size(std::forward<Span>(span))) {}

	const uchar *data() const {
		return _p;
	}

	constexpr size_t size() const {
		return _n;
	}

	void resize(size_t size) {
		if (!size) {
			reset();
			return;
		}
		if (!_p) {
			return;
		}
		_n = size;
	}

	void reset() {
		if (!_p) {
			return;
		}
		_p = nullptr;
		_n = 0;
	}

	template <typename... Args>
	void reset(Args &&... args) {
		RUA_SASSERT((std::is_constructible<bytes_view, Args...>::value));

		*this = bytes_view(std::forward<Args>(args)...);
	}

private:
	const uchar *_p;
	size_t _n;
};

template <typename Span>
inline bytes_view const_bytes_base<Span>::slice(
	ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const {
	assert(end_offset_from_begin >= begin_offset);

	if (begin_offset == end_offset_from_begin) {
		return nullptr;
	}
	return bytes_view(
		_this()->data() + begin_offset,
		static_cast<size_t>(end_offset_from_begin - begin_offset));
}

template <typename Span>
inline bytes_view const_bytes_base<Span>::slice(ptrdiff_t begin_offset) const {
	return slice(begin_offset, _this()->size());
}

template <typename Span>
inline bytes_view const_bytes_base<Span>::operator()(
	ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const {
	return slice(begin_offset, end_offset_from_begin);
}

template <typename Span>
inline bytes_view
const_bytes_base<Span>::operator()(ptrdiff_t begin_offset) const {
	return slice(begin_offset);
}

template <typename Span>
inline bool const_bytes_base<Span>::eq(bytes_view target) const {
	auto sz = _this()->size();
	if (sz != target.size()) {
		return false;
	}
	auto a = _this()->data();
	auto b = target.data();
	if (a == b) {
		return true;
	}
	if (!a || !b) {
		return false;
	}
	return bit_eq(a, b, sz);
}

template <typename Span>
inline bool const_bytes_base<Span>::operator==(bytes_view target) const {
	return eq(target);
}

class bytes_ref : public bytes_base<bytes_ref> {
public:
	constexpr bytes_ref(std::nullptr_t = nullptr) : _p(nullptr), _n(0) {}

	constexpr bytes_ref(uchar *ptr, size_t size) :
		_p(ptr), _n(ptr ? size : 0) {}

	template <
		typename Span,
		typename SpanTraits = span_traits<Span &&>,
		typename = enable_if_t<
			!std::is_base_of<bytes_ref, decay_t<Span>>::value &&
			std::is_same<typename SpanTraits::value_type, uchar>::value &&
			!std::is_const<typename SpanTraits::element_type>::value>>
	constexpr bytes_ref(Span &&span) :
		bytes_ref(
			span_data(std::forward<Span>(span)),
			span_size(std::forward<Span>(span))) {}

	uchar *data() {
		return _p;
	}

	const uchar *data() const {
		return _p;
	}

	constexpr size_t size() const {
		return _n;
	}

	void resize(size_t size) {
		if (!size) {
			reset();
			return;
		}
		if (!_p) {
			return;
		}
		_n = size;
	}

	void reset() {
		if (!_p) {
			return;
		}
		_p = nullptr;
		_n = 0;
	}

	template <typename... Args>
	void reset(Args &&... args) {
		RUA_SASSERT((std::is_constructible<bytes_ref, Args...>::value));

		*this = bytes_ref(std::forward<Args>(args)...);
	}

private:
	uchar *_p;
	size_t _n;
};

template <typename Span>
inline bytes_view bytes_base<Span>::slice(
	ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const {
	assert(end_offset_from_begin >= begin_offset);

	if (begin_offset == end_offset_from_begin) {
		return nullptr;
	}
	return bytes_view(
		_this()->data() + begin_offset,
		static_cast<size_t>(end_offset_from_begin - begin_offset));
}

template <typename Span>
inline bytes_view bytes_base<Span>::slice(ptrdiff_t begin_offset) const {
	return slice(begin_offset, _this()->size());
}

template <typename Span>
inline bytes_ref bytes_base<Span>::slice(
	ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) {
	assert(end_offset_from_begin >= begin_offset);

	if (begin_offset == end_offset_from_begin) {
		return nullptr;
	}
	return bytes_ref(
		_this()->data() + begin_offset,
		static_cast<size_t>(end_offset_from_begin - begin_offset));
}

template <typename Span>
inline bytes_ref bytes_base<Span>::slice(ptrdiff_t begin_offset) {
	return slice(begin_offset, _this()->size());
}

template <typename Span>
inline bytes_view bytes_base<Span>::operator()(
	ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) const {
	return slice(begin_offset, end_offset_from_begin);
}

template <typename Span>
inline bytes_view bytes_base<Span>::operator()(ptrdiff_t begin_offset) const {
	return slice(begin_offset);
}

template <typename Span>
inline bytes_ref bytes_base<Span>::operator()(
	ptrdiff_t begin_offset, ptrdiff_t end_offset_from_begin) {
	return slice(begin_offset, end_offset_from_begin);
}

template <typename Span>
inline bytes_ref bytes_base<Span>::operator()(ptrdiff_t begin_offset) {
	return slice(begin_offset);
}

template <typename Span>
template <typename... DestArgs>
inline size_t const_bytes_base<Span>::copy_to(DestArgs &&... dest) const {
	bytes_ref dest_ref(std::forward<DestArgs>(dest)...);
	auto cp_sz =
		_this()->size() < dest_ref.size() ? _this()->size() : dest_ref.size();
	if (!cp_sz) {
		return 0;
	}
	memcpy(dest_ref.data(), _this()->data(), cp_sz);
	return cp_sz;
}

template <typename Span>
template <typename... SrcArgs>
inline size_t bytes_base<Span>::copy_from(SrcArgs &&... src) const {
	return bytes_view(std::forward<SrcArgs>(src)...).copy_to(*_this());
}

template <typename E, bool IsConst = std::is_const<E>::value>
inline enable_if_t<
	size_of<E>::value,
	conditional_t<std::is_const<E>::value, bytes_view, bytes_ref>>
as_bytes(E *ptr) {
	return {
		reinterpret_cast<conditional_t<IsConst, const uchar, uchar> *>(ptr),
		sizeof(E)};
}

template <typename T>
inline bytes_view as_bytes(T *ptr, size_t size) {
	return {reinterpret_cast<const uchar *>(ptr), size};
}

template <typename T, typename R, typename... Args>
inline bytes_view as_bytes(R (T::*mem_fn_ptr)(Args...), size_t size) {
	return {bit_cast<const uchar *>(mem_fn_ptr), size};
}

template <
	typename T,
	bool IsConst = std::is_const<remove_reference_t<T>>::value>
inline enable_if_t<
	!std::is_convertible<T &&, generic_ptr>::value && !is_span<T &&>::value,
	conditional_t<IsConst, bytes_view, bytes_ref>>
as_bytes(T &&ref, size_t size = sizeof(T)) {
	return {
		reinterpret_cast<conditional_t<IsConst, const uchar, uchar> *>(&ref),
		size};
}

template <
	typename Span,
	typename SpanTraits = span_traits<Span &&>,
	bool IsConst = std::is_const<typename SpanTraits::element_type>::value>
inline enable_if_t<
	!std::is_base_of<bytes_view, decay_t<Span>>::value &&
		(!std::is_array<remove_reference_t<Span>>::value ||
		 (!std::is_same<typename SpanTraits::value_type, char>::value &&
		  !std::is_same<typename SpanTraits::value_type, wchar_t>::value)),
	conditional_t<IsConst, bytes_view, bytes_ref>>
as_bytes(Span &&span) {
	return {
		reinterpret_cast<conditional_t<IsConst, const uchar, uchar> *>(
			span_data(std::forward<Span>(span))),
		span_size(std::forward<Span>(span)) *
			sizeof(typename SpanTraits::element_type)};
}

inline bytes_view as_bytes(const char *c_str) {
	return as_bytes(string_view(c_str));
}

inline bytes_ref as_bytes(char *c_str) {
	return {reinterpret_cast<uchar *>(c_str), c_str_len(c_str)};
}

inline bytes_view as_bytes(const wchar_t *c_str) {
	return as_bytes(wstring_view(c_str));
}

inline bytes_ref as_bytes(wchar_t *c_str) {
	return {
		reinterpret_cast<uchar *>(c_str), c_str_len(c_str) * sizeof(wchar_t)};
}

template <typename T>
inline bytes_ref as_writable_bytes(T &&data) {
	auto b = as_bytes(data);
	return bytes_ref(const_cast<uchar *>(b.data()), b.size());
}

template <typename Bytes>
inline string_view as_string(const const_bytes_base<Bytes> &b) {
	return static_cast<const Bytes &>(b).size()
			   ? string_view(
					 reinterpret_cast<const char *>(
						 static_cast<const Bytes &>(b).data()),
					 static_cast<const Bytes &>(b).size())
			   : "";
}

class bytes : public bytes_ref {
public:
	constexpr bytes(std::nullptr_t = nullptr) : bytes_ref() {}

	explicit bytes(size_t size) {
		if (!size) {
			return;
		}
		_alloc(size);
	}

	template <
		typename... Args,
		typename ArgsFront = decay_t<front_t<Args...>>,
		typename = enable_if_t<
			(sizeof...(Args) > 1) ||
			(!std::is_base_of<bytes, ArgsFront>::value &&
			 !std::is_integral<ArgsFront>::value)>>
	bytes(Args &&... copy_src) {
		bytes_view bv(std::forward<Args>(copy_src)...);
		if (!bv.size()) {
			return;
		}
		_alloc(bv.size());
		copy_from(bv);
	}

	bytes(std::initializer_list<uchar> il) : bytes(il.begin(), il.size()) {}

	~bytes() {
		reset();
	}

	bytes(const bytes &src) : bytes(bytes_view(src)) {}

	bytes(bytes &&src) : bytes_ref(static_cast<bytes_ref &&>(std::move(src))) {
		if (src) {
			src.bytes_ref::reset();
		}
	}

	RUA_OVERLOAD_ASSIGNMENT(bytes)

	bytes &operator+=(bytes_view tail) {
		auto old_sz = size();
		auto new_sz = old_sz + tail.size();

		resize(new_sz);
		assert(size() == new_sz);

		slice(old_sz).copy_from(tail);

		return *this;
	}

	void resize(size_t size) {
		if (!data()) {
			reset(size);
			return;
		}
		if (_capacity() > size) {
			bytes_ref::resize(size);
			return;
		}
		bytes new_byts(size);
		new_byts.copy_from(*this);
		*this = std::move(new_byts);
	}

	size_t capacity() const {
		return data() ? _capacity() : 0;
	}

	void reserve(size_t cap) {
		auto sz = size();
		resize(cap);
		resize(sz);
	}

	void reset() {
		if (!data()) {
			return;
		}
		delete (data() - sizeof(size_t));
		bytes_ref::reset();
	}

	void reset(size_t size) {
		if (!size) {
			reset();
			return;
		}
		if (capacity() > size) {
			bytes_ref::resize(size);
			return;
		}
		reset();
		_alloc(size);
	}

private:
	void _alloc(size_t size) {
		bytes_ref::reset(
			new uchar[sizeof(size_t) + size] + sizeof(size_t), size);
		bit_set<size_t>(data() - sizeof(size_t), size);
	}

	size_t _capacity() const {
		return bit_get<size_t>(data() - sizeof(size_t));
	}
};

inline bytes operator+(bytes_view a, bytes_view b) {
	bytes r(a.size() + b.size());
	r.copy_from(a);
	r.slice(a.size()).copy_from(b);
	return r;
}

template <typename T>
inline bytes to_bytes(T &&data) {
	return as_bytes(data);
}

class bytes_pattern {
public:
	bytes_pattern() = default;

	template <
		typename... Args,
		typename ArgsFront = decay_t<front_t<Args...>>,
		typename = enable_if_t<
			(sizeof...(Args) > 1) ||
			(!std::is_base_of<bytes_pattern, ArgsFront>::value &&
			 std::is_constructible<bytes, Args &&...>::value)>>
	bytes_pattern(Args &&... bytes_args) :
		_v(std::forward<Args>(bytes_args)...) {}

	template <
		typename IntList,
		typename IntListTraits = span_traits<IntList &&>,
		typename Int = typename IntListTraits::value_type,
		typename = enable_if_t<
			std::is_integral<Int>::value && (sizeof(Int) > sizeof(uchar))>>
	bytes_pattern(IntList &&li) {
		_input(li);
	}

	bytes_pattern(std::initializer_list<uint16_t> il) {
		_input(il);
	}

	size_t size() const {
		return _v.size();
	}

	bytes_view view() const {
		return _v;
	}

	bytes_view mask() const {
		return _m;
	}

	struct sub_area_t {
		size_t offset, size;
	};

	const std::vector<sub_area_t> &variable_areas() const {
		return _vas;
	}

	inline bool contains(bytes_view byts) const {
		auto sz = byts.size();
		if (sz != size()) {
			return false;
		}
		return bit_contains(view().data(), mask().data(), byts.data(), sz);
	}

private:
	bytes _v, _m;
	std::vector<sub_area_t> _vas;

	template <typename IntList>
	void _input(IntList &&li) {
		_v.reset(li.size());
		auto begin = li.begin();
		auto in_va = false;
		size_t i;
		for (i = 0; i < _v.size(); ++i) {
			auto val = begin[i];
			if (val < 256) {
				if (in_va) {
					in_va = false;
					_vas.back().size = i - _vas.back().offset;
				}
				assert(i < _v.size());
				_v[i] = static_cast<uchar>(val);
			} else {
				if (!in_va) {
					in_va = true;
					_vas.emplace_back(sub_area_t{i, 0});
				}
				assert(i < _v.size());
				_v[i] = 0;
				if (!_m) {
					_m.reset(_v.size());
					memset(_m.data(), 255, _m.size());
				}
				_m[i] = 0;
			}
		}
		if (in_va) {
			_vas.back().size = i - _vas.back().offset;
		}
	}
};

template <typename Span>
inline bool
operator>=(const bytes_pattern &byts_pat, const const_bytes_base<Span> &byts) {
	return byts_pat.contains(byts);
}

template <typename Span>
inline bool
operator<=(const const_bytes_base<Span> &byts, const bytes_pattern &byts_pat) {
	return byts_pat.contains(byts);
}

template <typename Span>
inline size_t const_bytes_base<Span>::index_of(
	const bytes_pattern &find_data, size_t start_pos) const {

	auto sz = _this()->size();
	auto f_sz = find_data.size();
	if (sz < f_sz) {
		return nullpos;
	}

	auto begin = _this()->data() + start_pos;
	auto end = begin + _this()->size() - f_sz;
	auto f_begin = find_data.view().data();

	auto m_begin = find_data.mask().data();
	if (m_begin) {
		assert(find_data.variable_areas().size());

		for (auto it = begin; it != end; ++it) {
			if (bit_contains(f_begin, m_begin, it, f_sz)) {
				return it - begin;
			}
		}
		return nullpos;
	}

	if (sz == f_sz) {
		if (bit_eq(begin, f_begin, sz)) {
			return 0;
		}
		return nullpos;
	}

	size_t h = 0;
	size_t f_h = 0;
	for (size_t i = 0; i < f_sz; ++i) {
		f_h += f_begin[i];
	}

	auto f_back_ix = f_sz - 1;
	for (auto it = begin; it != end; ++it) {
		if (it == begin) {
			for (size_t j = 0; j < f_sz; ++j) {
				h += it[j];
			}
		} else {
			h -= it[-1];
			h += it[f_back_ix];
		}
		if (h != f_h) {
			continue;
		}
		if (bit_eq(it, f_begin, f_sz)) {
			return it - begin;
		}
	}
	return nullpos;
}

template <typename Span>
inline size_t const_bytes_base<Span>::last_index_of(
	const bytes_pattern &find_data, size_t start_pos) const {

	auto sz = _this()->size();
	auto f_sz = find_data.size();
	if (sz < f_sz) {
		return nullpos;
	}

	if (start_pos >= sz) {
		start_pos = sz;
	}

	auto begin = _this()->data();
	auto begin_before = begin - 1;
	auto end = _this()->data() + start_pos - f_sz;
	auto f_begin = find_data.view().data();

	auto m_begin = find_data.mask().data();
	if (m_begin) {
		assert(find_data.variable_areas().size());

		for (auto it = end; it != begin_before; --it) {
			if (bit_contains(f_begin, m_begin, it, f_sz)) {
				return it - begin;
			}
		}
		return nullpos;
	}

	if (sz == f_sz) {
		if (bit_eq(end, f_begin, sz)) {
			return 0;
		}
		return nullpos;
	}

	size_t h = 0;
	size_t f_h = 0;
	for (size_t i = 0; i < f_sz; ++i) {
		f_h += f_begin[i];
	}

	for (auto it = end; it != begin_before; --it) {
		if (it == end) {
			for (size_t j = 0; j < f_sz; ++j) {
				h += it[j];
			}
		} else {
			h -= it[f_sz];
			h += it[0];
		}
		if (h != f_h) {
			continue;
		}
		if (bit_eq(it, f_begin, f_sz)) {
			return it - begin;
		}
	}
	return nullpos;
}

template <typename Bytes>
class basic_bytes_finder : private wandering_iterator {
public:
	static basic_bytes_finder
	find(Bytes place, bytes_pattern find_data, size_t start_pos = 0) {
		auto pos = place.index_of(find_data, start_pos);
		if (pos == nullpos) {
			return basic_bytes_finder();
		}
		return basic_bytes_finder(place, find_data, pos);
	}

	static basic_bytes_finder
	rfind(Bytes place, bytes_pattern find_data, size_t start_pos = nullpos) {
		auto pos = place.last_index_of(find_data, start_pos);
		if (pos == nullpos) {
			return basic_bytes_finder();
		}
		return basic_bytes_finder(place, find_data, pos);
	}

	basic_bytes_finder() = default;

	operator bool() const {
		return _found.size();
	}

	Bytes &operator*() {
		return _found;
	}

	const Bytes &operator*() const {
		return _found;
	}

	Bytes *operator->() {
		return &_found;
	}

	const Bytes *operator->() const {
		return &_found;
	}

	Bytes operator[](size_t ix) {
		auto &sub = _find_data.variable_areas()[ix];
		return _found(sub.offset, sub.offset + sub.size);
	}

	bytes_view operator[](size_t ix) const {
		auto &sub = _find_data.variable_areas()[ix];
		return _found(sub.offset, sub.offset + sub.size);
	}

	basic_bytes_finder &operator++() {
		return *this = basic_bytes_finder::find(
				   _place, std::move(_find_data), pos());
	}

	basic_bytes_finder operator++(int) {
		basic_bytes_finder old(*this);
		++*this;
		return old;
	}

	basic_bytes_finder &operator--() {
		return *this = basic_bytes_finder::rfind(
				   _place, std::move(_find_data), pos());
	}

	basic_bytes_finder operator--(int) {
		basic_bytes_finder old(*this);
		--*this;
		return old;
	}

	size_t pos() const {
		assert(_found);
		return _found.data() - _place.data();
	}

	Bytes befores() {
		return _place(0, pos());
	}

	bytes_view befores() const {
		return _place(0, pos());
	}

	Bytes afters() {
		return _place(pos() + _found.size());
	}

	bytes_view afters() const {
		return _place(pos() + _found.size());
	}

private:
	Bytes _place;
	bytes_pattern _find_data;
	Bytes _found;

	basic_bytes_finder(Bytes place, bytes_pattern find_data, size_t found_pos) :
		_place(place),
		_find_data(std::move(find_data)),
		_found(place(found_pos, found_pos + _find_data.size())) {}
};

template <typename Span>
inline const_bytes_finder
const_bytes_base<Span>::find(bytes_pattern find_data, size_t start_pos) const {
	return const_bytes_finder::find(*_this(), std::move(find_data), start_pos);
}

template <typename Span>
inline const_bytes_rfinder
const_bytes_base<Span>::rfind(bytes_pattern find_data, size_t start_pos) const {
	return const_bytes_rfinder(
		const_bytes_finder::rfind(*_this(), std::move(find_data), start_pos));
}

template <typename Span>
inline bytes_finder
bytes_base<Span>::find(bytes_pattern find_data, size_t start_pos) {
	return bytes_finder::find(*_this(), std::move(find_data), start_pos);
}

template <typename Span>
inline const_bytes_finder
bytes_base<Span>::find(bytes_pattern find_data, size_t start_pos) const {
	return const_bytes_finder::find(*_this(), std::move(find_data), start_pos);
}

template <typename Span>
inline bytes_rfinder
bytes_base<Span>::rfind(bytes_pattern find_data, size_t start_pos) {
	return bytes_rfinder(
		bytes_finder::rfind(*_this(), std::move(find_data), start_pos));
}

template <typename Span>
inline const_bytes_rfinder
bytes_base<Span>::rfind(bytes_pattern find_data, size_t start_pos) const {
	return const_bytes_rfinder(
		const_bytes_finder::rfind(*_this(), std::move(find_data), start_pos));
}

template <
	typename Derived,
	size_t Size = !std::is_same<Derived, void>::value ? size_of<Derived>::value
													  : nmax<size_t>()>
class enable_bytes_accessor
	: public bytes_base<enable_bytes_accessor<Derived, Size>> {
public:
	uchar *data() {
		return reinterpret_cast<uchar *>(static_cast<Derived *>(this));
	}

	const uchar *data() const {
		return reinterpret_cast<const uchar *>(
			static_cast<const Derived *>(this));
	}

	static constexpr size_t size() {
		return Size;
	}

protected:
	constexpr enable_bytes_accessor() = default;
};

template <>
inline uchar *enable_bytes_accessor<void>::data() {
	return reinterpret_cast<uchar *>(this);
}

template <>
inline const uchar *enable_bytes_accessor<void>::data() const {
	return reinterpret_cast<const uchar *>(this);
}

template <size_t Size, size_t Align = Size + Size % 2>
class bytes_block : public bytes_base<bytes_block<Size, Align>> {
public:
	uchar *data() {
		return &_raw[0];
	}

	const uchar *data() const {
		return &_raw[0];
	}

	static constexpr size_t size() {
		return Size;
	}

private:
	alignas(Align) uchar _raw[Size];
};

} // namespace rua

#endif
