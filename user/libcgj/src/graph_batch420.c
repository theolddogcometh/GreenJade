/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch420: isolate lowest set bit of a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_isolate_lsb(uint64_t x);
 *     — Return x & -x: the single lowest set bit of x, or 0 if x == 0.
 *       Pure integer two's-complement isolate; no builtins.
 *   uint64_t __gj_isolate_lsb  (alias)
 *   __libcgj_batch420_marker = "libcgj-batch420"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch420_marker[] = "libcgj-batch420";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_isolate_lsb — keep only the least-significant 1-bit of x.
 *
 * x: value to isolate
 *
 * Two's-complement identity: x & -x (unsigned wrap of -x is defined).
 * Examples: 0 → 0; 0x10 → 0x10; 0x12 → 0x2; 0xffffffffffffffff → 1.
 */
uint64_t
gj_isolate_lsb(uint64_t u64X)
{
	return u64X & (uint64_t)(0ull - u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_isolate_lsb(uint64_t u64X)
    __attribute__((alias("gj_isolate_lsb")));
