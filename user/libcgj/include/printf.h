/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped printf.h (custom printf hooks). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <printf.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

struct printf_info {
    int prec;
    int width;
    wchar_t spec;
    unsigned is_long_double : 1;
    unsigned is_short : 1;
    unsigned is_long : 1;
    unsigned alt : 1;
    unsigned space : 1;
    unsigned left : 1;
    unsigned showsign : 1;
    unsigned group : 1;
    unsigned extra : 1;
    unsigned is_char : 1;
    unsigned wide : 1;
    unsigned i18n : 1;
    unsigned is_binary128 : 1;
    unsigned __pad : 3;
    unsigned short user;
    wchar_t pad;
};

typedef int (*printf_function)(FILE *pStream, const struct printf_info *pInfo,
                               const void *const *ppArgs);
typedef int (*printf_arginfo_size_function)(const struct printf_info *pInfo,
                                            size_t n, int *pArgtypes,
                                            int *pSize);
typedef int (*printf_arginfo_function)(const struct printf_info *pInfo,
                                       size_t n, int *pArgtypes);
typedef void (*printf_va_arg_function)(void *pMem, va_list *pAp);

#define PA_INT        0
#define PA_CHAR       1
#define PA_WCHAR      2
#define PA_STRING     3
#define PA_WSTRING    4
#define PA_POINTER    5
#define PA_FLOAT      6
#define PA_DOUBLE     7
#define PA_LAST       8
#define PA_FLAG_LONG_LONG (1 << 8)
#define PA_FLAG_LONG      (1 << 9)
#define PA_FLAG_SHORT     (1 << 10)
#define PA_FLAG_PTR       (1 << 11)

int register_printf_function(int nSpec, printf_function pfn,
                             printf_arginfo_function pfnInfo);
int register_printf_specifier(int nSpec, printf_function pfn,
                              printf_arginfo_size_function pfnInfo);
int register_printf_modifier(const wchar_t *pStr);
int register_printf_type(printf_va_arg_function pfn);
size_t parse_printf_format(const char *szFmt, size_t n, int *pArgtypes);

#ifdef __cplusplus
}
#endif
