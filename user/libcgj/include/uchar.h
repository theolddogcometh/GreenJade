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
 *
 * Soft residual (C2 libcgj header; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft path  = C11 char16/char32 restartable conversion (UTF-8 bring-up)
 *   product    = full locale multibyte state machines (OPEN; not here)
 *   catalog    = c16rtomb c32rtomb mbrtoc16 mbrtoc32 + char16_t/char32_t
 *   honesty    = Soft!=product; complements wchar soft surface; no .ko AC
 *   Bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION from this unit.
 * greppable: libcgj: soft residual uchar
 * greppable: libcgj: soft residual uchar Soft!=product
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN.
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
