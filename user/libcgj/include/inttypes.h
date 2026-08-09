/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped inttypes.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <inttypes.h>. Constants and type sizes
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
 * Soft residual (C2 libcgj inttypes.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = PRI*/SCN* format macros + imaxabs/imaxdiv/strtoimax subset
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = format catalog != product printf completeness; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: inttypes soft residual Soft!=product
 */
#pragma once

#include <stdint.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    intmax_t quot;
    intmax_t rem;
} imaxdiv_t;

intmax_t  imaxabs(intmax_t n);
imaxdiv_t imaxdiv(intmax_t nNumer, intmax_t nDenom);
intmax_t  strtoimax(const char *sz, char **ppEnd, int nBase);
uintmax_t strtoumax(const char *sz, char **ppEnd, int nBase);
intmax_t  wcstoimax(const wchar_t *pwcs, wchar_t **ppEnd, int nBase);
uintmax_t wcstoumax(const wchar_t *pwcs, wchar_t **ppEnd, int nBase);

/* printf/scanf macros (common subset) */
#define PRId64 "ld"
#define PRIu64 "lu"
#define PRIx64 "lx"
#define PRIdMAX "ld"
#define PRIuMAX "lu"
#define SCNd64 "ld"
#define SCNu64 "lu"

#ifdef __cplusplus
}
#endif
