/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3704: floor base-10 logarithm for uint64_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u64_log10_floor_u(uint64_t x);
 *     - Floor of log10(x) for x >= 1:
 *       gj_u64_log10_floor_u(1)==0, gj_u64_log10_floor_u(9)==0,
 *       gj_u64_log10_floor_u(10)==1, gj_u64_log10_floor_u(UINT64_MAX)==19.
 *       Returns 0 when x == 0 (defined sentinel; log10(0) undefined).
 *   uint32_t __gj_u64_log10_floor_u  (alias)
 *   __libcgj_batch3704_marker = "libcgj-batch3704"
 *
 * Exclusive continuum CREATE-ONLY (3701-3710). Distinct from
 * gj_u32_log10_floor_u (batch3703). Unique log10_floor_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3704_marker[] = "libcgj-batch3704";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Floor log10 via power-of-ten threshold ladder through 10^19.
 * Returns 0 for x == 0 (sentinel). Result fits in [0, 19].
 */
static uint32_t
b3704_log10_floor(uint64_t u64X)
{
	if (u64X < 10ULL) {
		return 0u;
	}
	if (u64X < 100ULL) {
		return 1u;
	}
	if (u64X < 1000ULL) {
		return 2u;
	}
	if (u64X < 10000ULL) {
		return 3u;
	}
	if (u64X < 100000ULL) {
		return 4u;
	}
	if (u64X < 1000000ULL) {
		return 5u;
	}
	if (u64X < 10000000ULL) {
		return 6u;
	}
	if (u64X < 100000000ULL) {
		return 7u;
	}
	if (u64X < 1000000000ULL) {
		return 8u;
	}
	if (u64X < 10000000000ULL) {
		return 9u;
	}
	if (u64X < 100000000000ULL) {
		return 10u;
	}
	if (u64X < 1000000000000ULL) {
		return 11u;
	}
	if (u64X < 10000000000000ULL) {
		return 12u;
	}
	if (u64X < 100000000000000ULL) {
		return 13u;
	}
	if (u64X < 1000000000000000ULL) {
		return 14u;
	}
	if (u64X < 10000000000000000ULL) {
		return 15u;
	}
	if (u64X < 100000000000000000ULL) {
		return 16u;
	}
	if (u64X < 1000000000000000000ULL) {
		return 17u;
	}
	if (u64X < 10000000000000000000ULL) {
		return 18u;
	}
	return 19u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_log10_floor_u - floor(log10(x)); 0 maps to 0.
 *
 * x: unsigned 64-bit value
 *
 * Returns floor(log10(x)) for x >= 1; 0 when x == 0. No parent wires.
 */
uint32_t
gj_u64_log10_floor_u(uint64_t x)
{
	(void)NULL;
	return b3704_log10_floor(x);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u64_log10_floor_u(uint64_t x)
    __attribute__((alias("gj_u64_log10_floor_u")));
