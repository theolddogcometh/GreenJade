/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3703: floor base-10 logarithm for uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_log10_floor_u(uint32_t x);
 *     - Floor of log10(x) for x >= 1:
 *       gj_u32_log10_floor_u(1)==0, gj_u32_log10_floor_u(9)==0,
 *       gj_u32_log10_floor_u(10)==1, gj_u32_log10_floor_u(99)==1,
 *       gj_u32_log10_floor_u(100)==2, gj_u32_log10_floor_u(UINT32_MAX)==9.
 *       Returns 0 when x == 0 (defined sentinel; log10(0) undefined).
 *   uint32_t __gj_u32_log10_floor_u  (alias)
 *   __libcgj_batch3703_marker = "libcgj-batch3703"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Unique log10_floor_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3703_marker[] = "libcgj-batch3703";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor log10 via power-of-ten threshold ladder. O(1) compare depth.
 * Returns 0 for x == 0 (sentinel).
 */
static uint32_t
b3703_log10_floor(uint32_t uX)
{
	if (uX < 10u) {
		return 0u;
	}
	if (uX < 100u) {
		return 1u;
	}
	if (uX < 1000u) {
		return 2u;
	}
	if (uX < 10000u) {
		return 3u;
	}
	if (uX < 100000u) {
		return 4u;
	}
	if (uX < 1000000u) {
		return 5u;
	}
	if (uX < 10000000u) {
		return 6u;
	}
	if (uX < 100000000u) {
		return 7u;
	}
	if (uX < 1000000000u) {
		return 8u;
	}
	return 9u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_log10_floor_u - floor(log10(x)); 0 maps to 0.
 *
 * x: unsigned 32-bit value
 *
 * Returns floor(log10(x)) for x >= 1; 0 when x == 0. No parent wires.
 */
uint32_t
gj_u32_log10_floor_u(uint32_t x)
{
	(void)NULL;
	return b3703_log10_floor(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_log10_floor_u(uint32_t x)
    __attribute__((alias("gj_u32_log10_floor_u")));
