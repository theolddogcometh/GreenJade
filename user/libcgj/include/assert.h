/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <assert.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * ISO C assert macro and glibc-shaped __assert_fail for failed diagnostics.
 * With NDEBUG defined, assert expands to a no-op (expression not evaluated
 * for side effects — same as standard C).
 *
 * Design notes
 * ------------
 * Failure path is noreturn: message then abort-shaped exit. Use for
 * programmer invariants, not for recoverable runtime errors (prefer errno).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/** Report failed assertion and terminate (expression string, file, line, func). */
void __assert_fail(const char *szExpr, const char *szFile, unsigned nLine,
                   const char *szFunc)
    __attribute__((noreturn));

#ifdef NDEBUG
#define assert(expr) ((void)0)
#else
#define assert(expr)                                                           \
    ((expr) ? (void)0                                                          \
            : __assert_fail(#expr, __FILE__, __LINE__, __func__))
#endif

#ifdef __cplusplus
}
#endif
