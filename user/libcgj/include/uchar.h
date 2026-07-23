/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room C11 uchar.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <uchar.h>. Constants and type sizes
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

#include <stddef.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef unsigned short char16_t;
typedef unsigned int   char32_t;
#endif

size_t c16rtomb(char *pS, char16_t c16, mbstate_t *pSt);
size_t c32rtomb(char *pS, char32_t c32, mbstate_t *pSt);
size_t mbrtoc16(char16_t *pC16, const char *pS, size_t n, mbstate_t *pSt);
size_t mbrtoc32(char32_t *pC32, const char *pS, size_t n, mbstate_t *pSt);

#ifdef __cplusplus
}
#endif
