#ifndef _RUA_INTERFACE_PTR_HPP
#define _RUA_INTERFACE_PTR_HPP

#include "macros.hpp"
#include "types.hpp"

#include <cassert>
#include <memory>

namespace rua {

template <typename T>
class interface_ptr : public enable_type_info {
public:
	constexpr interface_ptr(std::nullptr_t = nullptr) :
		enable_type_info(type_id<std::nullptr_t>()),
		_raw_ptr(nullptr),
		_shared_ptr() {}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	interface_ptr(SameBase *raw_ptr) {
		if (!raw_ptr) {
			_raw_ptr = nullptr;
			_reset_type<std::nullptr_t>();
			return;
		}
		_reset_type<SameBase>();
		_raw_ptr = static_cast<T *>(raw_ptr);
	}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	interface_ptr(SameBase &lv) : interface_ptr(&lv) {}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	interface_ptr(SameBase &&rv) {
		_reset_type<SameBase>();
		new (&_shared_ptr) std::shared_ptr<T>(std::static_pointer_cast<T>(
			std::make_shared<SameBase>(std::move(rv))));
		_raw_ptr = _shared_ptr.get();
	}

	interface_ptr(const std::shared_ptr<T> &base_shared_ptr_lv) {
		if (!base_shared_ptr_lv) {
			_raw_ptr = nullptr;
			_reset_type<std::nullptr_t>();
			return;
		}
		_reset_type<T>();
		new (&_shared_ptr) std::shared_ptr<T>(base_shared_ptr_lv);
		_raw_ptr = _shared_ptr.get();
	}

	interface_ptr(std::shared_ptr<T> &&base_shared_ptr_rv) {
		if (!base_shared_ptr_rv) {
			_raw_ptr = nullptr;
			_reset_type<std::nullptr_t>();
			return;
		}
		_reset_type<T>();
		new (&_shared_ptr) std::shared_ptr<T>(std::move(base_shared_ptr_rv));
		_raw_ptr = _shared_ptr.get();
	}

	template <RUA_DERIVED_CONCEPT(T, Derived)>
	interface_ptr(const std::shared_ptr<Derived> &derived_shared_ptr_lv) {
		if (!derived_shared_ptr_lv) {
			_raw_ptr = nullptr;
			_reset_type<std::nullptr_t>();
			return;
		}
		_reset_type<Derived>();
		new (&_shared_ptr) std::shared_ptr<T>(
			std::static_pointer_cast<T>(derived_shared_ptr_lv));
		_raw_ptr = _shared_ptr.get();
	}

	template <RUA_DERIVED_CONCEPT(T, Derived)>
	interface_ptr(std::shared_ptr<Derived> &&derived_shared_ptr_rv) {
		if (!derived_shared_ptr_rv) {
			_raw_ptr = nullptr;
			_reset_type<std::nullptr_t>();
			return;
		}
		_reset_type<Derived>();
		new (&_shared_ptr) std::shared_ptr<T>(
			std::static_pointer_cast<T>(std::move(derived_shared_ptr_rv)));
		_raw_ptr = _shared_ptr.get();
	}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	interface_ptr(const std::unique_ptr<SameBase> &unique_ptr_lv) :
		interface_ptr(unique_ptr_lv.get()) {}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	interface_ptr(std::unique_ptr<SameBase> &&unique_ptr_rv) {
		if (!unique_ptr_rv) {
			_raw_ptr = nullptr;
			_reset_type<std::nullptr_t>();
			return;
		}
		_reset_type<SameBase>();
		new (&_shared_ptr)
			std::shared_ptr<T>(static_cast<T *>(unique_ptr_rv.release()));
		_raw_ptr = _shared_ptr.get();
	}

	template <RUA_DERIVED_CONCEPT(T, Derived)>
	interface_ptr(const interface_ptr<Derived> &derived_blended_ptr_lv) :
		enable_type_info(
			static_cast<const enable_type_info &>(derived_blended_ptr_lv)) {

		if (!derived_blended_ptr_lv) {
			_raw_ptr = nullptr;
			return;
		}

		auto src_base_ptr_shared_ptr = derived_blended_ptr_lv.get_shared();
		if (src_base_ptr_shared_ptr) {
			new (&_shared_ptr) std::shared_ptr<T>(std::static_pointer_cast<T>(
				std::move(src_base_ptr_shared_ptr)));
			_raw_ptr = _shared_ptr.get();
			return;
		}

		_raw_ptr = static_cast<T *>(derived_blended_ptr_lv.get());
	}

	template <RUA_DERIVED_CONCEPT(T, Derived)>
	interface_ptr(interface_ptr<Derived> &&derived_blended_ptr_rv) :
		enable_type_info(
			static_cast<const enable_type_info &>(derived_blended_ptr_rv)) {

		if (!derived_blended_ptr_rv) {
			_raw_ptr = nullptr;
			return;
		}

		auto src_base_ptr_shared_ptr = derived_blended_ptr_rv.release_shared();
		if (src_base_ptr_shared_ptr) {
			new (&_shared_ptr) std::shared_ptr<T>(std::static_pointer_cast<T>(
				std::move(src_base_ptr_shared_ptr)));
			_raw_ptr = _shared_ptr.get();
			return;
		}

		_raw_ptr = static_cast<T *>(derived_blended_ptr_rv.release());
	}

	interface_ptr(const interface_ptr &src) :
		enable_type_info(static_cast<const enable_type_info &>(src)) {

		_raw_ptr = src._raw_ptr;

		if (!_raw_ptr || !src._shared_ptr) {
			return;
		}

		_shared_ptr = src._shared_ptr;
	}

	interface_ptr(interface_ptr &&src) :
		enable_type_info(static_cast<const enable_type_info &>(src)) {

		_raw_ptr = src.release();
		if (_raw_ptr || !src) {
			return;
		}

		_shared_ptr = src.release_shared();
		_raw_ptr = _shared_ptr.get();
	}

	RUA_OVERLOAD_ASSIGNMENT(interface_ptr)

	~interface_ptr() {
		reset();
	}

	operator bool() const {
		return _raw_ptr;
	}

	bool operator==(const interface_ptr<T> &src) const {
		return _raw_ptr == src._raw_ptr;
	}

	bool operator!=(const interface_ptr<T> &src) const {
		return _raw_ptr != src._raw_ptr;
	}

	template <
		typename =
			enable_if_t<std::is_class<T>::value || std::is_union<T>::value>>
	T *operator->() const {
		assert(_raw_ptr);
		return _raw_ptr;
	}

	T &operator*() const {
		return *_raw_ptr;
	}

	T *get() const {
		return _raw_ptr;
	}

	T *release() {
		if (_shared_ptr) {
			return nullptr;
		}
		auto base_raw_ptr = _raw_ptr;
		_raw_ptr = nullptr;
		_reset_type<std::nullptr_t>();
		return base_raw_ptr;
	}

	std::shared_ptr<T> get_shared() const & {
		if (_shared_ptr) {
			return _shared_ptr;
		}
		return nullptr;
	}

	std::shared_ptr<T> release_shared() {
		if (!_shared_ptr) {
			return nullptr;
		}
		_raw_ptr = nullptr;
		_reset_type<std::nullptr_t>();
		return std::move(_shared_ptr);
	}

	std::shared_ptr<T> get_shared() && {
		return release_shared();
	}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	SameBase *as() const {
		assert(_raw_ptr);

		if (!type_is<SameBase>()) {
			return nullptr;
		}
		if (!_raw_ptr) {
			return nullptr;
		}
		return static_cast<SameBase *>(_raw_ptr);
	}

	template <RUA_IS_BASE_OF_CONCEPT(T, SameBase)>
	std::shared_ptr<SameBase> to_shared() const {
		assert(_raw_ptr);

		if (!type_is<SameBase>()) {
			return nullptr;
		}
		auto base_ptr = get_shared();
		if (!base_ptr) {
			return nullptr;
		}
		return std::static_pointer_cast<SameBase>(base_ptr);
	}

	void reset() {
		if (_shared_ptr) {
			_shared_ptr.reset();
		}
		_raw_ptr = nullptr;
		_reset_type<std::nullptr_t>();
	}

private:
	T *_raw_ptr;
	std::shared_ptr<T> _shared_ptr;
};

} // namespace rua

#endif
