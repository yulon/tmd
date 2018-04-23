#ifndef _RUA_EXEC_CONT_WIN32_HPP
#define _RUA_EXEC_CONT_WIN32_HPP

#include "../macros.hpp"

#if !defined(RUA_AMD64) && !defined(RUA_I386)
	#error rua::exec::cont: not supported this platform!
#endif

#include "../gnc/any_ptr.hpp"
#include "../gnc/any_word.hpp"
#include "../mem/protect.hpp"

#include <cstdint>
#include <initializer_list>
#include <cassert>

namespace rua {
	namespace exec {
		template <typename CodeBytes>
		any_ptr code_init(CodeBytes &&cb) {
			mem::protect(&cb, sizeof(cb), mem::protect_read | mem::protect_exec);
			return &cb;
		}

		// Reference from https://github.com/skywind3000/collection/tree/master/context

		static const uint8_t _cont_push_code[]{
			#ifdef RUA_AMD64
				#ifdef RUA_WIN64_FASTCALL
					0x48, 0x89, 0x51, 0x18, 0x48, 0x89, 0x01, 0x48, 0x89, 0x59, 0x08, 0x48, 0x89, 0x49, 0x10,
					0x48, 0x89, 0x71, 0x20, 0x48, 0x89, 0x79, 0x28, 0x48, 0x89, 0x61, 0x30, 0x48, 0x89, 0x69,
					0x38, 0x48, 0x8B, 0x14, 0x24, 0x48, 0x89, 0x51, 0x40, 0x9C, 0x5A, 0x48, 0x89, 0x51, 0x48,
					0x4C, 0x89, 0x41, 0x50, 0x4C, 0x89, 0x49, 0x58, 0x4C, 0x89, 0x51, 0x60, 0x4C, 0x89, 0x59,
					0x68, 0x4C, 0x89, 0x61, 0x70, 0x4C, 0x89, 0x69, 0x78, 0x4C, 0x89, 0xB1, 0x80, 0x00, 0x00,
					0x00, 0x4C, 0x89, 0xB9, 0x88, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x51, 0x18, 0x0F, 0xAE, 0x99,
					0x90, 0x00, 0x00, 0x00, 0xD9, 0xB1, 0x98, 0x00, 0x00, 0x00, 0x48, 0xC7, 0xC0, 0x01, 0x00,
					0x00, 0x00, 0xC3
				#else
					0x48, 0x89, 0x57, 0x18, 0x48, 0x89, 0x07, 0x48, 0x89, 0x5F, 0x08, 0x48, 0x89, 0x4F, 0x10,
					0x48, 0x89, 0x77, 0x20, 0x48, 0x89, 0x7F, 0x28, 0x48, 0x8D, 0x54, 0x24, 0x08, 0x48, 0x89,
					0x57, 0x30, 0x48, 0x89, 0x6F, 0x38, 0x48, 0x8B, 0x14, 0x24, 0x48, 0x89, 0x57, 0x40, 0x9C,
					0x5A, 0x48, 0x89, 0x57, 0x48, 0x4C, 0x89, 0x47, 0x50, 0x4C, 0x89, 0x4F, 0x58, 0x4C, 0x89,
					0x57, 0x60, 0x4C, 0x89, 0x5F, 0x68, 0x4C, 0x89, 0x67, 0x70, 0x4C, 0x89, 0x6F, 0x78, 0x4C,
					0x89, 0xB7, 0x80, 0x00, 0x00, 0x00, 0x4C, 0x89, 0xBF, 0x88, 0x00, 0x00, 0x00, 0x48, 0x8B,
					0x57, 0x18, 0x0F, 0xAE, 0x9F, 0x90, 0x00, 0x00, 0x00, 0xD9, 0xB7, 0x98, 0x00, 0x00, 0x00,
					0x48, 0xC7, 0xC0, 0x01, 0x00, 0x00, 0x00, 0xC3
				#endif
			#elif defined(RUA_I386)
				0x89, 0x44, 0x24, 0xFC, 0x8B, 0x44, 0x24, 0x04, 0x89, 0x58, 0x04, 0x8B, 0x5C, 0x24, 0xFC,
				0x89, 0x18, 0x89, 0x48, 0x08, 0x89, 0x50, 0x0C, 0x89, 0x70, 0x10, 0x89, 0x78, 0x14, 0x89,
				0x60, 0x18, 0x89, 0x68, 0x1C, 0x8B, 0x1C, 0x24, 0x89, 0x58, 0x20, 0x9C, 0x5B, 0x89, 0x58,
				0x24, 0xD9, 0x70, 0x2C, 0x8B, 0x58, 0x04, 0xB8, 0x01, 0x00, 0x00, 0x00, 0xC3
			#endif
		};

		static bool (*_cont_push)(any_ptr) = code_init(_cont_push_code);

		static const uint8_t _cont_pop_code[]{
			#ifdef RUA_AMD64
				#ifdef RUA_WIN64_FASTCALL
					0x48, 0x8B, 0x59, 0x08, 0x48, 0x8B, 0x71, 0x20, 0x48, 0x8B, 0x79, 0x28, 0x48, 0x8B, 0x61,
					0x30, 0x48, 0x8B, 0x69, 0x38, 0x48, 0x8B, 0x51, 0x40, 0x48, 0x89, 0x14, 0x24, 0x48, 0x8B,
					0x51, 0x48, 0x52, 0x9D, 0x4C, 0x8B, 0x41, 0x50, 0x4C, 0x8B, 0x49, 0x58, 0x4C, 0x8B, 0x51,
					0x60, 0x4C, 0x8B, 0x59, 0x68, 0x4C, 0x8B, 0x61, 0x70, 0x4C, 0x8B, 0x69, 0x78, 0x4C, 0x8B,
					0xB1, 0x80, 0x00, 0x00, 0x00, 0x4C, 0x8B, 0xB9, 0x88, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x51,
					0x18, 0x0F, 0xAE, 0x91, 0x90, 0x00, 0x00, 0x00, 0xD9, 0xA1, 0x98, 0x00, 0x00, 0x00, 0x48,
					0x8B, 0x49, 0x10, 0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC3
				#else
					0x48, 0x8B, 0x5F, 0x08, 0x48, 0x8B, 0x4F, 0x10, 0x48, 0x8B, 0x77, 0x20, 0x48, 0x8B, 0x67,
					0x30, 0x48, 0x8B, 0x6F, 0x38, 0x48, 0x8B, 0x57, 0x40, 0x52, 0x48, 0x8B, 0x57, 0x48, 0x52,
					0x9D, 0x4C, 0x8B, 0x47, 0x50, 0x4C, 0x8B, 0x4F, 0x58, 0x4C, 0x8B, 0x57, 0x60, 0x4C, 0x8B,
					0x5F, 0x68, 0x4C, 0x8B, 0x67, 0x70, 0x4C, 0x8B, 0x6F, 0x78, 0x4C, 0x8B, 0xB7, 0x80, 0x00,
					0x00, 0x00, 0x4C, 0x8B, 0xBF, 0x88, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x57, 0x18, 0x0F, 0xAE,
					0x97, 0x90, 0x00, 0x00, 0x00, 0xD9, 0xA7, 0x98, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x7F, 0x28,
					0x48, 0xC7, 0xC0, 0x00, 0x00, 0x00, 0x00, 0xC3
				#endif
			#elif defined(RUA_I386)
				0x8B, 0x44, 0x24, 0x04, 0x8B, 0x48, 0x08, 0x8B, 0x50, 0x0C, 0x8B, 0x70, 0x10, 0x8B, 0x78,
				0x14, 0x8B, 0x60, 0x18, 0x8B, 0x68, 0x1C, 0x8B, 0x58, 0x20, 0x89, 0x1C, 0x24, 0x8B, 0x58,
				0x24, 0x53, 0x9D, 0xD9, 0x60, 0x2C, 0x8B, 0x58, 0x04, 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3
			#endif
		};

		static bool (*_cont_pop)(any_ptr) = code_init(_cont_pop_code);

		class cont {
			public:
				cont() = default;

				cont(void (*func)(any_word), any_word func_param, any_ptr stack, size_t stack_size) {
					remake(func, func_param, stack, stack_size);
				}

				bool push() {
					return _cont_push(this);
				}

				void pop() const {
					_cont_pop(this);
				}

				bool push_and_pop(const cont &pre_pop) {
					if (push()) {
						pre_pop.pop();
						return true;
					}
					return false;
				}

				bool swap() {
					auto old = *this;
					if (push()) {
						old.pop();
						return true;
					}
					return false;
				}

				#ifdef RUA_AMD64
					void rebind(void (*func)(any_word), any_word func_param, any_ptr stack, size_t stack_size) {
						rsp = (stack + stack_size - 2 * sizeof(uintptr_t)).value();

						#ifdef RUA_WIN64_FASTCALL
							rcx
						#else
							rdi
						#endif
						= func_param;

						caller_rip = reinterpret_cast<uintptr_t>(func);
					}
				#elif defined(RUA_I386)
					void rebind(void (*func)(any_word), any_word func_param, any_ptr stack, size_t stack_size) {
						esp = (stack + stack_size - 4 * sizeof(uintptr_t)).value();
						reinterpret_cast<uintptr_t *>(esp)[2] = func_param;
						caller_eip = reinterpret_cast<uintptr_t>(func);
					}
				#endif

				void remake(void (*func)(any_word), any_word func_param, any_ptr stack, size_t stack_size) {
					push();
					rebind(func, func_param, stack, stack_size);
				}

				////////////////////////////////////////////////////////////////

				#ifdef RUA_AMD64
					uintptr_t rax; // 0
					uintptr_t rbx; // 8
					uintptr_t rcx; // 16
					uintptr_t rdx; // 24
					uintptr_t rsi; // 32
					uintptr_t rdi; // 40
					uintptr_t rsp; // 48
					uintptr_t rbp; // 56
					uintptr_t caller_rip; // 64
					uintptr_t rflags; // 72
					uintptr_t r8; // 80
					uintptr_t r9; // 88
					uintptr_t r10; // 96
					uintptr_t r11; // 104
					uintptr_t r12; // 112
					uintptr_t r13; // 120
					uintptr_t r14; // 128
					uintptr_t r15; // 136
					uintptr_t mxcsr; // 144
				#elif defined(RUA_I386)
					uintptr_t eax; // 0
					uintptr_t ebx; // 4
					uintptr_t ecx; // 8
					uintptr_t edx; // 12
					uintptr_t esi; // 16
					uintptr_t edi; // 20
					uintptr_t esp; // 24
					uintptr_t ebp; // 28
					uintptr_t caller_eip; // 32
					uintptr_t eflags; // 36
					uintptr_t mxcsr; // 40
				#endif

				uint8_t fpregs[32]; // 44(32) / 152(64)
				uint32_t reserved[18]; // 76(32) / 186(64)
		};
	}
}

#endif
