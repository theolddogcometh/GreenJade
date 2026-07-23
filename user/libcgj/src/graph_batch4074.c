/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4074: uint64_t square wave (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_square_wave_u(uint64_t phase, uint64_t period);
 *     - Digital square wave of the given period. Returns 1 for the
 *       first ceil(period/2) samples of each period and 0 for the
 *       remainder. period == 0 → 0 (soft guard).
 *   uint64_t __gj_u64_square_wave_u  (alias)
 *   __libcgj_batch4074_marker = "libcgj-batch4074"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u64_square_wave_u surface only; no multi-def. Distinct from
 * gj_u32_square_wave_u (batch4073). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4074_marker[] = "libcgj-batch4074";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Digital square wave: 1 in the first half of the period, 0 in the
 * second. Soft-guard period == 0 → 0.
 *
 * Overflow note: for period > UINT64_MAX/2 + 1, 2*t can wrap when t is
 * large. Cap the high-half test by comparing t against period/2 with a
 * remainder-of-period bias for odd periods via (period + 1) / 2.
 * Equivalent for all period values without a 2*t product.
 */
static uint64_t
b4074_square(uint64_t u64Phase, uint64_t u64Period)
{
	uint64_t u64T;
	uint64_t u64High;

	if (u64Period == 0ull) {
		return 0ull;
	}
	u64T = u64Phase % u64Period;
	/* High samples: first ceil(period/2) positions → 1. */
	u64High = (u64Period + 1ull) / 2ull;
	if (u64T < u64High) {
		return 1ull;
	}
	return 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_square_wave_u - unsigned 64-bit digital square wave.
 *
 * phase:  free-running phase sample
 * period: wave period in samples (0 → 0 soft guard)
 *
 * Returns 1 during the first half of each period, else 0.
 * No parent wires.
 */
uint64_t
gj_u64_square_wave_u(uint64_t u64Phase, uint64_t u64Period)
{
	(void)NULL;
	return b4074_square(u64Phase, u64Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_square_wave_u(uint64_t u64Phase, uint64_t u64Period)
    __attribute__((alias("gj_u64_square_wave_u")));
