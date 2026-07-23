/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3035: floor half of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_half_floor_u(uint64_t x);
 *     - Return floor(x / 2) as unsigned integer (logical right shift).
 *   uint64_t __gj_u64_half_floor_u  (alias)
 *   __libcgj_batch3035_marker = "libcgj-batch3035"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_u64_half_floor_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3035_marker[] = "libcgj-batch3035";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(x/2) via logical right shift. */
static uint64_t
b3035_half_floor(uint64_t u64X)
{
	return u64X >> 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_half_floor_u - floor division of x by 2.
 *
 * x: value to halve
 *
 * Returns x / 2 (truncate toward zero / floor for unsigned).
 */
uint64_t
gj_u64_half_floor_u(uint64_t u64X)
{
	(void)NULL;
	return b3035_half_floor(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_half_floor_u(uint64_t u64X)
    __attribute__((alias("gj_u64_half_floor_u")));
