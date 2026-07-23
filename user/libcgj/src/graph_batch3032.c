/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3032: ceiling half of uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_half_ceil_u(uint32_t x);
 *     - Return ceil(x / 2) as unsigned integer (no overflow path).
 *   uint32_t __gj_u32_half_ceil_u  (alias)
 *   __libcgj_batch3032_marker = "libcgj-batch3032"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_u32_half_ceil_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3032_marker[] = "libcgj-batch3032";

/* ---- freestanding helpers ---------------------------------------------- */

/* ceil(x/2) = (x >> 1) + (x & 1); safe for all uint32_t. */
static uint32_t
b3032_half_ceil(uint32_t u32X)
{
	return (u32X >> 1) + (u32X & 1u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_half_ceil_u - ceiling division of x by 2.
 *
 * x: value to halve
 *
 * Returns (x + 1) / 2 without intermediate overflow on UINT32_MAX.
 */
uint32_t
gj_u32_half_ceil_u(uint32_t u32X)
{
	(void)NULL;
	return b3032_half_ceil(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_half_ceil_u(uint32_t u32X)
    __attribute__((alias("gj_u32_half_ceil_u")));
