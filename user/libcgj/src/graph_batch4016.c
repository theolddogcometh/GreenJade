/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4016: isolate the highest set bit of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_isolate_msb_u(uint32_t x);
 *     - Return a mask with only the most-significant 1-bit of x kept, or
 *       0 if x == 0. Pure integer smear + isolate; no builtins.
 *   uint32_t __gj_u32_isolate_msb_u  (alias)
 *   __libcgj_batch4016_marker = "libcgj-batch4016"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_bitset64_isolate_msb (batch1634), gj_u32_blsi_u (batch3405), and
 * gj_u32_set_msb_u (batch4007, fixed bit 31) — unique
 * gj_u32_isolate_msb_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4016_marker[] = "libcgj-batch4016";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Isolate most-significant 1-bit: smear bits downward then peel off
 * the lower smear with (smear >> 1). Zero stays zero.
 */
static uint32_t
b4016_isolate_msb(uint32_t u32X)
{
	uint32_t u32V;

	if (u32X == 0u) {
		return 0u;
	}

	u32V = u32X;
	u32V |= u32V >> 1;
	u32V |= u32V >> 2;
	u32V |= u32V >> 4;
	u32V |= u32V >> 8;
	u32V |= u32V >> 16;
	return u32V - (u32V >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_isolate_msb_u - keep only the most-significant 1-bit of x.
 *
 * x: unsigned 32-bit value
 *
 * Examples: 0 -> 0; 0x10 -> 0x10; 0x12 -> 0x10; 0xffffffffu -> 0x80000000u.
 * No parent wires.
 */
uint32_t
gj_u32_isolate_msb_u(uint32_t u32X)
{
	(void)NULL;
	return b4016_isolate_msb(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_isolate_msb_u(uint32_t u32X)
    __attribute__((alias("gj_u32_isolate_msb_u")));
