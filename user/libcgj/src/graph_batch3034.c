/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3034: floor half of uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_half_floor_u(uint32_t x);
 *     - Return floor(x / 2) as unsigned integer (logical right shift).
 *   uint32_t __gj_u32_half_floor_u  (alias)
 *   __libcgj_batch3034_marker = "libcgj-batch3034"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_u32_half_floor_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3034_marker[] = "libcgj-batch3034";

/* ---- freestanding helpers ---------------------------------------------- */

/* floor(x/2) via logical right shift. */
static uint32_t
b3034_half_floor(uint32_t u32X)
{
	return u32X >> 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_half_floor_u - floor division of x by 2.
 *
 * x: value to halve
 *
 * Returns x / 2 (truncate toward zero / floor for unsigned).
 */
uint32_t
gj_u32_half_floor_u(uint32_t u32X)
{
	(void)NULL;
	return b3034_half_floor(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_half_floor_u(uint32_t u32X)
    __attribute__((alias("gj_u32_half_floor_u")));
