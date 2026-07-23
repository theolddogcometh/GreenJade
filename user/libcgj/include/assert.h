/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room assert.h. Not GNU glibc.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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
