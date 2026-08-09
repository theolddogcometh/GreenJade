/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room iconv (identity / Latin-1 / UTF-8). Not GNU libiconv/glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <iconv.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj iconv.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = iconv_open/iconv/iconv_close; identity / Latin-1 / UTF-8 subset
 *              (not full GNU libiconv repertoire)
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = charset soft != product locale completeness; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: iconv soft residual Soft!=product
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *iconv_t;

iconv_t iconv_open(const char *szTo, const char *szFrom);
int     iconv_close(iconv_t cd);
size_t  iconv(iconv_t cd, char **ppIn, size_t *pInLeft, char **ppOut,
              size_t *pOutLeft);

#ifdef __cplusplus
}
#endif
