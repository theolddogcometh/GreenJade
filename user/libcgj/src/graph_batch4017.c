/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4017: isolate the highest set bit of a uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_isolate_msb_u(uint64_t x);
 *     - Return a mask with only the most-significant 1-bit of x kept, or
 *       0 if x == 0. Pure integer smear + isolate; no builtins.
 *   uint64_t __gj_u64_isolate_msb_u  (alias)
 *   __libcgj_batch4017_marker = "libcgj-batch4017"
 *
 * Exclusive continuum CREATE-ONLY (4011-4020). Distinct from
 * gj_u32_isolate_msb_u (batch4016), gj_bitset64_isolate_msb (batch1634),
 * and gj_u64_set_msb_u (batch4008, fixed bit 63) — unique
 * gj_u64_isolate_msb_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4017_marker[] = "libcgj-batch4017";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Isolate most-significant 1-bit: smear bits downward then peel off
 * the lower smear with (smear >> 1). Zero stays zero.
 */
static uint64_t
b4017_isolate_msb(uint64_t u64X)
{
	uint64_t u64V;

	if (u64X == 0ull) {
		return 0ull;
	}

	u64V = u64X;
	u64V |= u64V >> 1;
	u64V |= u64V >> 2;
	u64V |= u64V >> 4;
	u64V |= u64V >> 8;
	u64V |= u64V >> 16;
	u64V |= u64V >> 32;
	return u64V - (u64V >> 1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_isolate_msb_u - keep only the most-significant 1-bit of x.
 *
 * x: unsigned 64-bit value
 *
 * Examples: 0 -> 0; 0x10 -> 0x10; 0x12 -> 0x10;
 *           ~0ull -> 0x8000000000000000ULL.
 * No parent wires.
 */
uint64_t
gj_u64_isolate_msb_u(uint64_t u64X)
{
	(void)NULL;
	return b4017_isolate_msb(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_isolate_msb_u(uint64_t u64X)
    __attribute__((alias("gj_u64_isolate_msb_u")));
