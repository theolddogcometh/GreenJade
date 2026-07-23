/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped err.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <err.h>. Constants and type sizes
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

#ifdef __cplusplus
extern "C" {
#endif

void err(int nEval, const char *szFmt, ...) __attribute__((noreturn, format(printf, 2, 3)));
void errx(int nEval, const char *szFmt, ...) __attribute__((noreturn, format(printf, 2, 3)));
void errc(int nEval, int nErr, const char *szFmt, ...)
    __attribute__((noreturn, format(printf, 3, 4)));
void warn(const char *szFmt, ...) __attribute__((format(printf, 1, 2)));
void warnx(const char *szFmt, ...) __attribute__((format(printf, 1, 2)));
void warnc(int nErr, const char *szFmt, ...) __attribute__((format(printf, 2, 3)));
void verr(int nEval, const char *szFmt, va_list ap) __attribute__((noreturn));
void verrx(int nEval, const char *szFmt, va_list ap) __attribute__((noreturn));
void vwarn(const char *szFmt, va_list ap);
void vwarnx(const char *szFmt, va_list ap);

#ifdef __cplusplus
}
#endif
