/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room sys/swap.h (swapon/swapoff).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/swap.h>. Constants and type sizes
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
 *   - swapon/swapoff graph; soft fills != product swap DoD
 *   - Soft!=product: libcgj sys/swap surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj sys/swap.h: soft residual lean Soft!=product
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SWAP_FLAG_PREFER       0x8000
#define SWAP_FLAG_DISCARD      0x10000
#define SWAP_FLAG_DISCARD_ONCE 0x20000
#define SWAP_FLAG_DISCARD_PAGES 0x40000

int swapon(const char *szPath, int nFlags);
int swapoff(const char *szPath);

#ifdef __cplusplus
}
#endif
