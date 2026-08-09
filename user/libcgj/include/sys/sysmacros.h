/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/sysmacros.h. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/sysmacros.h>. Constants and type sizes
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
 *   - major/minor/makedev macros; pure shape residual
 *   - Soft!=product: libcgj sys/sysmacros surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj sys/sysmacros.h: soft residual lean Soft!=product
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned int       major(dev_t dev);
unsigned int       minor(dev_t dev);
dev_t              makedev(unsigned int uMajor, unsigned int uMinor);
unsigned int       gnu_dev_major(unsigned long long uDev);
unsigned int       gnu_dev_minor(unsigned long long uDev);
unsigned long long gnu_dev_makedev(unsigned int uMajor, unsigned int uMinor);

#ifdef __cplusplus
}
#endif
