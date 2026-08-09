/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped utime.h. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <utime.h>. Constants and type sizes
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
 *   soft path  = utime/__utime via utimensat-shaped soft fill
 *   product    = full filesystem utimens product policy (OPEN; not here)
 *   catalog    = struct utimbuf + utime + __utime
 *   honesty    = Soft!=product; soft time touch != product store DoD close
 *   Bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION from this unit.
 * greppable: libcgj: soft residual utime
 * greppable: libcgj: soft residual utime Soft!=product
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN.
 */
#pragma once

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
    time_t actime;
    time_t modtime;
};

int utime(const char *szPath, const struct utimbuf *pTimes);
int __utime(const char *szPath, const struct utimbuf *pTimes);

#ifdef __cplusplus
}
#endif
