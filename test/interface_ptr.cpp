#include <rua/interface_ptr.hpp>

#include <rua/test.hpp>

#include <memory>

namespace {

rua::test _t("interface_ptr", "basic", []() {
	struct animal {
		virtual ~animal() = default;
		virtual size_t age() const = 0;
	};

	using animal_i = rua::interface_ptr<animal>;

	struct dog : virtual animal {
		virtual ~dog() = default;
		virtual size_t age() const {
			return 123;
		}
		size_t color() const {
			return 321;
		}
	};

	dog dog_lval;
	std::unique_ptr<dog> dog_unique_ptr_lval(new dog);

	////////////////////////////////////////////////////////////////////////////

	// save raw pointer
	animal_i aml1(dog_lval); // Impl &
	animal_i aml2(&dog_lval); // Impl *
	animal_i aml3(dog_unique_ptr_lval); // std::unique_ptr<Impl> &

	// save shared pointer
	animal_i aml4(std::make_shared<dog>()); // std::shared_ptr<Impl>
	animal_i aml5(dog{}); // Impl &&
	animal_i aml6(std::unique_ptr<dog>(new dog)); // std::unique_ptr<Impl> &&

	// save pointer type same as source jeko
	animal_i aml7(aml6); // jeko

	// null
	animal_i aml8;
	animal_i aml9(nullptr);

	////////////////////////////////////////////////////////////////////////////

	RUA_PANIC(aml1->age() == dog_lval.age());
	RUA_PANIC(aml1.to<dog>()->age() == dog_lval.age());
	RUA_PANIC(aml2->age() == dog_lval.age());
	RUA_PANIC(aml3->age() == dog_lval.age());
	RUA_PANIC(aml4->age() == dog_lval.age());
	RUA_PANIC(aml5->age() == dog_lval.age());
	RUA_PANIC(aml6->age() == dog_lval.age());
	RUA_PANIC(aml7->age() == dog_lval.age());
	RUA_PANIC(!aml8);
	RUA_PANIC(!aml9);
});

}