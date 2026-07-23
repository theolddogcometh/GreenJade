/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4073: uint32_t square wave (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_square_wave_u(uint32_t phase, uint32_t period);
 *     - Digital square wave of the given period. Returns 1 for the
 *       first ceil(period/2) samples of each period and 0 for the
 *       remainder. period == 0 → 0 (soft guard).
 *   uint32_t __gj_u32_square_wave_u  (alias)
 *   __libcgj_batch4073_marker = "libcgj-batch4073"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u32_square_wave_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4073_marker[] = "libcgj-batch4073";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Digital square wave: 1 in the first half of the period, 0 in the
 * second. High half length is ceil(period/2). Soft-guard period==0 → 0.
 * Avoids 2*t (overflow when t > UINT32_MAX/2).
 */
static uint32_t
b4073_square(uint32_t u32Phase, uint32_t u32Period)
{
	uint32_t u32T;
	uint32_t u32High;

	if (u32Period == 0u) {
		return 0u;
	}
	u32T = u32Phase % u32Period;
	u32High = (u32Period + 1u) / 2u;
	if (u32T < u32High) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_square_wave_u - unsigned 32-bit digital square wave.
 *
 * phase:  free-running phase sample
 * period: wave period in samples (0 → 0 soft guard)
 *
 * Returns 1 during the first half of each period, else 0.
 * No parent wires.
 */
uint32_t
gj_u32_square_wave_u(uint32_t u32Phase, uint32_t u32Period)
{
	(void)NULL;
	return b4073_square(u32Phase, u32Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_square_wave_u(uint32_t u32Phase, uint32_t u32Period)
    __attribute__((alias("gj_u32_square_wave_u")));
