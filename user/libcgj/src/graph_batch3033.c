/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3033: ceiling half of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_half_ceil_u(uint64_t x);
 *     - Return ceil(x / 2) as unsigned integer (no overflow path).
 *   uint64_t __gj_u64_half_ceil_u  (alias)
 *   __libcgj_batch3033_marker = "libcgj-batch3033"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_u64_half_ceil_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3033_marker[] = "libcgj-batch3033";

/* ---- freestanding helpers ---------------------------------------------- */

/* ceil(x/2) = (x >> 1) + (x & 1); safe for all uint64_t. */
static uint64_t
b3033_half_ceil(uint64_t u64X)
{
	return (u64X >> 1) + (u64X & 1ull);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_half_ceil_u - ceiling division of x by 2.
 *
 * x: value to halve
 *
 * Returns (x + 1) / 2 without intermediate overflow on UINT64_MAX.
 */
uint64_t
gj_u64_half_ceil_u(uint64_t u64X)
{
	(void)NULL;
	return b3033_half_ceil(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_half_ceil_u(uint64_t u64X)
    __attribute__((alias("gj_u64_half_ceil_u")));
