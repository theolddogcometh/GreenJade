/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped fnmatch (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <fnmatch.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj fnmatch.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = fnmatch + FNM_* flags (PATHNAME/NOESCAPE/PERIOD/CASEFOLD subset)
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = glob match soft != product shell completeness; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: fnmatch soft residual Soft!=product
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define FNM_NOMATCH     1
#define FNM_PATHNAME    (1 << 0)
#define FNM_NOESCAPE    (1 << 1)
#define FNM_PERIOD      (1 << 2)
#define FNM_CASEFOLD    (1 << 4)

int fnmatch(const char *szPattern, const char *szString, int nFlags);

#ifdef __cplusplus
}
#endif
