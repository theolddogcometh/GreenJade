/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4379: hardware ticks to nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_ticks_to_ns_u(uint64_t ticks, uint64_t hz);
 *     - Return floor(ticks * 1000000000 / hz). hz == 0 → 0.
 *       Saturates at UINT64_MAX when the scaled quotient would overflow.
 *   uint64_t __gj_ticks_to_ns_u  (alias)
 *   __libcgj_batch4379_marker = "libcgj-batch4379"
 *
 * Exclusive continuum CREATE-ONLY (4371-4380). Distinct from
 * gj_ticks_to_ms (batch1975) — unique gj_ticks_to_ns_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4379_marker[] = "libcgj-batch4379";

/* Nanoseconds scale factor for tick conversion (1 s = 1e9 ns). */
#define B4379_NS_SCALE  1000000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4379_ticks_to_ns — floor(ticks * 1e9 / hz) with careful overflow.
 *
 * Prefers (ticks * 1e9) / hz when the product fits. For large ticks,
 * rewrites as floor(ticks/hz)*1e9 + floor((ticks%hz)*1e9/hz),
 * saturating intermediate multiplies.
 */
static uint64_t
b4379_ticks_to_ns(uint64_t u64Ticks, uint64_t u64Hz)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64Hz == 0ull) {
		return 0ull;
	}
	if (u64Ticks <= (UINT64_MAX / B4379_NS_SCALE)) {
		return (u64Ticks * B4379_NS_SCALE) / u64Hz;
	}
	u64Whole = u64Ticks / u64Hz;
	u64Rem = u64Ticks % u64Hz;
	if (u64Whole > (UINT64_MAX / B4379_NS_SCALE)) {
		return UINT64_MAX;
	}
	u64Whole *= B4379_NS_SCALE;
	if (u64Rem <= (UINT64_MAX / B4379_NS_SCALE)) {
		u64Part = (u64Rem * B4379_NS_SCALE) / u64Hz;
	} else {
		/*
		 * rem > UINT64_MAX/1e9 implies hz is enormous; the
		 * fractional contribution is still in [0, 1e9-1]. Drop
		 * it rather than overflow (saturate whole only).
		 */
		u64Part = 0ull;
	}
	if (u64Whole > (UINT64_MAX - u64Part)) {
		return UINT64_MAX;
	}
	return u64Whole + u64Part;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ticks_to_ns_u - convert timer ticks to nanoseconds.
 *
 * ticks: elapsed hardware/soft timer ticks
 * hz:    tick frequency in ticks-per-second (0 → 0)
 *
 * Returns floor(ticks * 1000000000 / hz), saturating at UINT64_MAX.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_ticks_to_ns_u(uint64_t u64Ticks, uint64_t u64Hz)
{
	(void)NULL;
	return b4379_ticks_to_ns(u64Ticks, u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ticks_to_ns_u(uint64_t u64Ticks, uint64_t u64Hz)
    __attribute__((alias("gj_ticks_to_ns_u")));
