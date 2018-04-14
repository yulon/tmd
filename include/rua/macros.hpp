#ifndef _RUA_MACROS_HPP
#define _RUA_MACROS_HPP

#define RUA_CPP_17 201703L
#define RUA_CPP_14 201402L
#define RUA_CPP_11 201103L
#define RUA_CPP_98 199711L

#ifdef _MSC_VER
	#if __cplusplus > RUA_CPP_98
		#define RUA_CPP __cplusplus
	#elif defined(_MSVC_LANG)
		#define RUA_CPP _MSVC_LANG
	#else
		#define RUA_CPP RUA_CPP_98
	#endif
#else
	#define RUA_CPP __cplusplus
#endif

#ifdef __cpp_constexpr
	#if __cpp_constexpr >= 200704
		#define RUA_CONSTEXPR_SUPPORTED
	#endif
	#if __cpp_constexpr >= 201304
		#define RUA_CONSTEXPR_14_SUPPORTED
	#endif
	#if __cpp_constexpr >= 201603
		#define RUA_CONSTEXPR_LM_SUPPORTED
	#endif
#endif

#ifdef RUA_CONSTEXPR_SUPPORTED
	#define RUA_CONSTEXPR constexpr
#else
	#define RUA_CONSTEXPR
#endif

#ifdef RUA_CONSTEXPR_14_SUPPORTED
	#define RUA_CONSTEXPR_14 constexpr
#else
	#define RUA_CONSTEXPR_14
#endif

#ifdef RUA_CONSTEXPR_LM_SUPPORTED
	#define RUA_CONSTEXPR_LM constexpr
#else
	#define RUA_CONSTEXPR_LM
#endif

#ifdef __cpp_if_constexpr
	#define RUA_CONSTEXPR_IF_SUPPORTED
#endif

#ifdef RUA_CONSTEXPR_IF_SUPPORTED
	#define RUA_CONSTEXPR_IF constexpr
#else
	#define RUA_CONSTEXPR_IF
#endif

#ifdef RUA_CONSTEXPR_IF_SUPPORTED
	#define RUA_CONSTEXPR_17_SUPPORTED
#endif

#ifdef RUA_CONSTEXPR_17_SUPPORTED
	#define RUA_CONSTEXPR_17 constexpr
#else
	#define RUA_CONSTEXPR_17
#endif

#if defined(__cpp_inline_variables) && __cpp_inline_variables >= 201606
	#define RUA_INLINE_VAR inline
#else
	#define RUA_INLINE_VAR
#endif

#if defined(__cpp_static_assert) && __cpp_static_assert >= 201411
	#define RUA_STATIC_ASSERT(cond) static_assert(cond)
#elif RUA_CPP >= RUA_CPP_11
	#define RUA_STATIC_ASSERT(cond) static_assert(cond, #cond)
#else
	#define RUA_STATIC_ASSERT(cond) assert(cond)
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	#define RUA_UNIX

	#if defined(__APPLE__) && defined(__MACH__)
		#define RUA_MAC
	#endif
#endif

#if RUA_CPP >= RUA_CPP_17
	#define RUA_FALLTHROUGH [[fallthrough]]
#elif defined(__GNUC__)
	#if RUA_CPP >= 11
		#define RUA_FALLTHROUGH [[gnu::fallthrough]]
	#else
		#define RUA_FALLTHROUGH __attribute__ ((fallthrough))
	#endif
#else
	#define RUA_FALLTHROUGH
#endif

#endif