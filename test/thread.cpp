#include <rua/sync.hpp>
#include <rua/thread.hpp>

#include <catch.hpp>

#include <string>

TEST_CASE("thread", "[thread]") {
	static std::string r;

	rua::thread([]() mutable {
		rua::sleep(100);
		r = "ok";
	})
		.wait_for_exit();

	REQUIRE(r == "ok");
}

TEST_CASE("use chan on thread", "[thread]") {
	static rua::chan<std::string> ch;

	rua::thread([]() mutable {
		rua::sleep(100);
		ch << "ok";
	});

	REQUIRE(ch.pop() == "ok");
}
