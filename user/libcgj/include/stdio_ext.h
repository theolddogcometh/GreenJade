/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped stdio_ext.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <stdio_ext.h>. Constants and type sizes
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
 * Soft residual lean (this unit; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   - __f* locking/buffer probes; soft fills may ENOSYS until hybrid ABI
 *   - Soft!=product: libcgj stdio_ext surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj stdio_ext.h: soft residual lean Soft!=product
 */
#pragma once

#include <stdio.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FSETLOCKING_QUERY    0
#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2

size_t __fbufsize(FILE *pF);
int    __flbf(FILE *pF);
size_t __fpending(FILE *pF);
void   __fpurge(FILE *pF);
void   fpurge(FILE *pF);
int    __freadable(FILE *pF);
int    __fwritable(FILE *pF);
int    __freading(FILE *pF);
int    __fwriting(FILE *pF);
int    __fsetlocking(FILE *pF, int nType);

#ifdef __cplusplus
}
#endif
