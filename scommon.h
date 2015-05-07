#ifndef SCOMMON_H
#define SCOMMON_H
#ifdef __cplusplus
extern "C" {
#endif

/*
 * scommon.h
 *
 * Common definitions.
 *
 * Copyright (c) 2015 F. Aragon. All rights reserved.
 */

#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#include <crtdefs.h>
	#include <BaseTsd.h>
	#include <malloc.h>
	#include <io.h>
	typedef SSIZE_T ssize_t;
	/* MS VC don't support UTF-8 in sprintf, not even using _setmbcp(65001)
	 * and "multi-byte character set" compile mode.
	 */
	#define S_NOT_UTF8_SPRINTF
#elif defined(__BCC__)
	#define TOY_C_COMPILER
#elif defined(__SDCC)
	/* e.g. sdcc -mz180 allinone.c */
	#define TOY_C_COMPILER
#elif defined(__WATCOMC__)
	#include <io.h>
	#include <wctype.h>
	#include <alloca.h>
	#if defined(__HUGE__) || defined(__LARGE___) || defined(__MEDIUM__) || \
	    defined(__COMPACT__) || defined(__SMALL__) || defined(__TINY__)
		#define TOY_C_COMPILER
	#else
	#endif
#elif defined(__DMC__)
	#include <unistd.h>
	typedef int ssize_t;
#else
	#include <sys/types.h>
	#include <alloca.h>
	#include <unistd.h>
#endif
#ifdef TOY_C_COMPILER
	typedef long sint32_t;
	typedef unsigned long suint32_t;
#else
	#include <wctype.h>
	#include <wchar.h>
	typedef int sint32_t;
	typedef unsigned int suint32_t;
#endif
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>

/*
 * Context
 */

#if __STDC_VERSION__ >= 199901L || __cplusplus >= 19971L || \
	defined(__GNUC__) || defined (__TINYC__) || _MSC_VER >= 1800 || \
	defined(__DMC__)
	#define S_MODERN_COMPILER
	#define S_USE_VA_ARGS
#endif

#if defined(S_MODERN_COMPILER) && !defined(__MSC_VER)
	#define S_POSIX_LOCALE_SUPPORT
#endif

#ifndef offsetof
	#define offsetof(s, m) ((size_t)(&((s *)0)->m))
#endif

#if defined(__CYGWIN__) && !defined(UINTPTR_MAX)
	#define UINTPTR_MAX 0xffffffff
#endif

#if UINTPTR_MAX == 0xffff
	#define S_BPWORD 2 /* 16-bit CPU */
#elif UINTPTR_MAX == 0xffffffff
	#define S_BPWORD 4 /* 32 */
#elif UINTPTR_MAX == 0xffffffffffffffff
	#define S_BPWORD 8 /* 64 */
#else /* assume 128-bit CPU */
	#define S_BPWORD 16
#endif

/*
 * Prefetch stuff
 */

#if defined(MSVC) && defined(_M_X86)
	#include <emmintrin.h>
	#define S_PREFETCH_R(address) 					\
		if (address)						\
			_mm_prefetch((char *)(address), _MM_HINT_T0)
	#define S_PREFETCH_W(address) S_PREFETCH_R(address)
#endif
#if defined(__GNUC__)
	#define S_PREFETCH_GNU(address, rw)		\
		if (address)				\
			__builtin_prefetch(address, rw)
	#define S_PREFETCH_R(address) S_PREFETCH_GNU(address, 0)
	#define S_PREFETCH_W(address) S_PREFETCH_GNU(address, 1)
#endif
#ifndef S_PREFETCH_R
	#define S_PREFETCH_R(address)
#endif
#ifndef S_PREFETCH_W
	#define S_PREFETCH_W(address)
#endif

/*
 * Variable argument helpers
 */

#ifdef S_USE_VA_ARGS /* purposes: 1) count parameters, 2) parameter syntax check */
	/* void *, char *, wchar_t * */
	#define S_NARGS_W(...) (sizeof((void * []){__VA_ARGS__})/sizeof(void *))
	#define S_NARGS_R(...) (sizeof((const void * []){__VA_ARGS__})/sizeof(const void *))
	#define S_NARGS_CW(...) (sizeof((char * []){__VA_ARGS__})/sizeof(char *))
	#define S_NARGS_CR(...) (sizeof((const char * []){__VA_ARGS__})/sizeof(const char *))
	#define S_NARGS_WW(...) (sizeof((wchar_t * []){__VA_ARGS__})/sizeof(wchar_t *))
	#define S_NARGS_WR(...) (sizeof((const wchar_t * []){__VA_ARGS__})/sizeof(const wchar_t *))
	/* ss_t * */
	#define S_NARGS_SW(...) (sizeof((ss_t * []){__VA_ARGS__})/sizeof(ss_t *))
	#define S_NARGS_SR(...) (sizeof((const ss_t * []){__VA_ARGS__})/sizeof(const ss_t *))
	#define S_NARGS_SPW(...) (sizeof((ss_t ** []){__VA_ARGS__})/sizeof(ss_t **))
	#define S_NARGS_SPR(...) (sizeof((const ss_t ** []){__VA_ARGS__})/sizeof(const ss_t **))
	/* sv_t * */
	#define S_NARGS_SVW(...) (sizeof((sv_t * []){__VA_ARGS__})/sizeof(sv_t *))
	#define S_NARGS_SVR(...) (sizeof((const sv_t * []){__VA_ARGS__})/sizeof(const sv_t *))
	#define S_NARGS_SVPW(...) (sizeof((sv_t ** []){__VA_ARGS__})/sizeof(sv_t **))
	#define S_NARGS_SVPR(...) (sizeof((const sv_t ** []){__VA_ARGS__})/sizeof(const sv_t **))
	/* st_t * */
	#define S_NARGS_STW(...) (sizeof((st_t * []){__VA_ARGS__})/sizeof(st_t *))
	#define S_NARGS_STR(...) (sizeof((const st_t * []){__VA_ARGS__})/sizeof(const st_t *))
	#define S_NARGS_STPW(...) (sizeof((st_t ** []){__VA_ARGS__})/sizeof(st_t **))
	#define S_NARGS_STPR(...) (sizeof((const st_t ** []){__VA_ARGS__})/sizeof(const st_t **))
	/* sdm_t */
	#define S_NARGS_SDMW(...) (sizeof((sdm_t * []){__VA_ARGS__})/sizeof(sdm_t *))
	#define S_NARGS_SDMR(...) (sizeof((const sdm_t * []){__VA_ARGS__})/sizeof(const sdm_t *))
	#define S_NARGS_SDMPW(...) (sizeof((sdm_t ** []){__VA_ARGS__})/sizeof(sdm_t **))
	#define S_NARGS_SDMPR(...) (sizeof((const sdm_t ** []){__VA_ARGS__})/sizeof(const sdm_t **))

#endif

/*
 * Constants
 */

#define S_NPOS		((size_t)-1)
#define S_NULL_C	"(null)"
#define S_NULL_WC	L"(null)"
#define S_TRUE		(1)
#define S_FALSE		(0)
#define S_SIZET_MAX	(UINTPTR_MAX)

#ifdef _MSC_VER
	#if S_BPWORD == 8
		#define FMT_ZI "%I64i"
		#define FMT_ZU "%I64u"
		#define FMT_I FMT_ZI
		#define FMTSSZ_T __int64
		#define FMTSZ_T unsigned __int64
	#else
		#define FMT_ZI "%i"
		#define FMT_ZU "%u"
		#define FMT_I "%I64i"
		#define FMTSSZ_T int
		#define FMTSZ_T unsigned int
	#endif
#else
	#define FMT_ZI "%zi"
	#define FMT_ZU "%zu"
	#if S_BPWORD >= 8
		#define FMT_I FMT_ZI
	#else
		#define FMT_I "%lli"
	#endif
	#define FMTSZ_T ssize_t
	#define FMTSSZ_T size_t
#endif

/*
 * Macros
 */

#define S_MIN(a, b) ((a) < (b) ? (a) : (b))
#define S_MAX(a, b) ((a) > (b) ? (a) : (b))
#define S_MIN3(a, b, c) S_MIN(S_MIN(a, b), (c))
#define S_ROL32(a, c) ((a) << (c) | (a) >> (32 - (c)))
#define S_ROR32(a, c) ((a) >> (c) | (a) << (32 - (c)))
#define S_BSWAP32(a) ((a) << 24 | (a) >> 24 | ((a) & 0xff00) << 8 | ((a) & 0xff0000) >> 8)
#define RETURN_IF(a, v) if (a) return (v)
#define ASSERT_RETURN_IF(a, v) S_ASSERT(!(a)); if (a) return (v)

/* debug and alloc counter */
#ifdef S_DEBUG
	#define S_ASSERT(a)						     \
		!(a) &&							     \
		fprintf(stderr, "S_ASSERT[function: %s, line: %i]: '%s'\n", \
		__FUNCTION__, __LINE__, #a)
	#define S_ERROR(msg) \
		fprintf(stderr, "%s: %s\n", __FUNCTION__, msg)
	#define S_PROFILE_ALLOC_CALL dbg_cnt_alloc_calls++
	extern size_t dbg_cnt_alloc_calls;      /* alloc or realloc calls */
#else
	#define S_ASSERT(a)
	#define S_ERROR(msg)
	#define S_PROFILE_ALLOC_CALL
#endif

/*
 * Types
 */

#if defined(_MSC_VER) || defined(__WATCOMC__)
	typedef __int64 sint_t;
	typedef unsigned __int64 suint_t;
#elif	LONG_MAX == INT_MAX /* 32 bit or 64 bit (LLP64) mode */
	#ifdef S_MODERN_COMPILER
		typedef long long sint_t;
		typedef unsigned long long suint_t;
	#else /* no 64 bit container support: */
		typedef long sint_t;
		typedef unsigned long suint_t;
	#endif
#else /* 64 bit mode (LP64) */
	#ifdef S_MODERN_COMPILER
		typedef ssize_t sint_t;
		typedef size_t suint_t;
	#else
		typedef long sint_t;
		typedef unsigned long suint_t;
	#endif
#endif

#define SINT_MIN (8LL << ((sizeof(sint_t) * 8) - 4))
#define SINT_MAX (~SINT_MIN)

#define SINT32_MAX ((sint32_t)(0x7fffffff))
#define SINT32_MIN ((sint32_t)(0x80000000))
#define SUINT32_MAX 0xffffffff
#define SUINT32_MIN 0

typedef unsigned char sbool_t;

#ifdef _DEBUG
	#define S_DEBUG
#endif

/*
 * Standard-compliance workarounds
 */

#ifdef _MSC_VER 
#define snprintf sprintf_s
#define posix_open _open
#define posix_read _read
#define posix_write _write
#define posix_close _close
#else
#define posix_open open
#define posix_read read
#define posix_write write
#define posix_close close
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif

/*
 * Helper functions intended to be inlined
 */

static void s_move_elems(void *t, const size_t t_off, const void *s, const size_t s_off, const size_t n, const size_t e_size)
{
        memmove((char *)t + t_off * e_size, (const char *)s + s_off * e_size, n * e_size);
}

static void s_copy_elems(void *t, const size_t t_off, const void *s, const size_t s_off, const size_t n, const size_t e_size)
{
        memcpy((char *)t + t_off * e_size, (const char *)s + s_off * e_size, n * e_size);
}

static size_t s_load_size_t(const void *aligned_base_address, const size_t offset)
{
	return *(size_t *)(((char *)aligned_base_address) + offset);
}

static sbool_t s_size_t_overflow(const size_t a, const size_t b)
{
	return a > (S_SIZET_MAX - b) ? S_TRUE : S_FALSE;
}

/*
 * Integer log2(N) approximation
 */

static unsigned slog2(suint_t i)
{
	unsigned o = 0, test;
	#define SLOG2STEP(mask, bits)				\
			test = !!(i & mask);			\
			o |= test * bits;			\
			i = test * (i >> bits) | !test * i;
#if INTPTR_MAX >= INT64_MAX
	SLOG2STEP(0xffffffff00000000, 32);
#endif
#if INTPTR_MAX >= INT32_MAX
	SLOG2STEP(0xffff0000, 16);
#endif
	SLOG2STEP(0xff00, 8);
	SLOG2STEP(0xf0, 4);
	SLOG2STEP(0x0c, 2);
	SLOG2STEP(0x02, 1);
	#undef SLOG2STEP
	return o;
}

#ifdef __cplusplus
};      /* extern "C" { */
#endif
#endif	/* SCOMMON_H */

