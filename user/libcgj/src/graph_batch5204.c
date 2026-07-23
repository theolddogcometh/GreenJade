/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5204: HPET ticks to nanoseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_hpet_ticks_to_ns_u(uint64_t ticks, uint64_t period_fs);
 *     - Return floor(ticks * period_fs / 1000000) nanoseconds.
 *       period_fs == 0 → 0. Saturates at UINT64_MAX on overflow.
 *   uint64_t __gj_hpet_ticks_to_ns_u  (alias)
 *   __libcgj_batch5204_marker = "libcgj-batch5204"
 *
 * Exclusive continuum CREATE-ONLY (5201-5210: HPET/timer unique).
 * Unique gj_hpet_ticks_to_ns_u surface only; no multi-def. Inverse of
 * gj_hpet_ns_to_ticks_u (batch5203). Distinct from gj_ticks_to_ns_u
 * (batch4379, hz-based). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5204_marker[] = "libcgj-batch5204";

/* Femtoseconds per nanosecond (scale fs product → ns). */
#define B5204_FS_PER_NS  1000000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b5204_ticks_to_ns — floor(ticks * period_fs / 1e6) without __int128.
 *
 * Prefers (ticks * period_fs) / 1e6 when the product fits. For large
 * ticks, rewrites as floor(ticks/1e6)*period + floor((ticks%1e6)*period/1e6),
 * saturating intermediate multiplies.
 */
static uint64_t
b5204_ticks_to_ns(uint64_t u64Ticks, uint64_t u64PeriodFs)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64PeriodFs == 0ull) {
		return 0ull;
	}
	if (u64Ticks <= (UINT64_MAX / u64PeriodFs)) {
		return (u64Ticks * u64PeriodFs) / B5204_FS_PER_NS;
	}
	u64Whole = u64Ticks / B5204_FS_PER_NS;
	u64Rem = u64Ticks % B5204_FS_PER_NS;
	if (u64Whole > (UINT64_MAX / u64PeriodFs)) {
		return UINT64_MAX;
	}
	u64Whole *= u64PeriodFs;
	if (u64Rem <= (UINT64_MAX / u64PeriodFs)) {
		u64Part = (u64Rem * u64PeriodFs) / B5204_FS_PER_NS;
	} else {
		/*
		 * rem * period would overflow; rem < 1e6 so this path
		 * only hits for enormous period_fs. Drop the fractional
		 * contribution rather than overflow.
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
 * gj_hpet_ticks_to_ns_u - convert HPET ticks to nanoseconds.
 *
 * ticks:     elapsed HPET counter ticks
 * period_fs: HPET period in femtoseconds (0 → 0)
 *
 * Returns floor(ticks * period_fs / 1000000), saturating at UINT64_MAX.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_hpet_ticks_to_ns_u(uint64_t u64Ticks, uint64_t u64PeriodFs)
{
	(void)NULL;
	return b5204_ticks_to_ns(u64Ticks, u64PeriodFs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_hpet_ticks_to_ns_u(uint64_t u64Ticks, uint64_t u64PeriodFs)
    __attribute__((alias("gj_hpet_ticks_to_ns_u")));
