/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4607: timer ticks to milliseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_to_ms_u(uint64_t ticks, uint64_t hz);
 *     - Return floor(ticks * 1000 / hz). hz == 0 → 0. Saturates at
 *       UINT64_MAX when the scaled quotient would overflow.
 *   uint64_t __gj_tick_to_ms_u  (alias)
 *   __libcgj_batch4607_marker = "libcgj-batch4607"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Distinct from
 * gj_ticks_to_ms (batch1975) / gj_ticks_to_ns_u (batch4379) — unique
 * gj_tick_to_ms_u surface only; no multi-def. Inverse shape of
 * gj_tick_from_ms_u (batch4606). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4607_marker[] = "libcgj-batch4607";

/* Milliseconds scale factor for tick conversion. */
#define B4607_MS_SCALE  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4607_to_ms — floor(ticks * 1000 / hz) with saturation.
 *
 * Prefers (ticks * 1000) / hz when the product fits. For large ticks,
 * rewrites as floor(ticks/hz)*1000 + floor((ticks%hz)*1000/hz).
 */
static uint64_t
b4607_to_ms(uint64_t u64Ticks, uint64_t u64Hz)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64Hz == 0ull) {
		return 0ull;
	}
	if (u64Ticks <= (UINT64_MAX / B4607_MS_SCALE)) {
		return (u64Ticks * B4607_MS_SCALE) / u64Hz;
	}
	u64Whole = u64Ticks / u64Hz;
	u64Rem = u64Ticks % u64Hz;
	if (u64Whole > (UINT64_MAX / B4607_MS_SCALE)) {
		return UINT64_MAX;
	}
	u64Whole *= B4607_MS_SCALE;
	if (u64Rem <= (UINT64_MAX / B4607_MS_SCALE)) {
		u64Part = (u64Rem * B4607_MS_SCALE) / u64Hz;
	} else {
		/*
		 * rem > UINT64_MAX/1000 implies hz is enormous; the
		 * fractional contribution is still in [0, 999]. Drop
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
 * gj_tick_to_ms_u - convert timer ticks to milliseconds.
 *
 * ticks: elapsed hardware/soft timer ticks
 * hz:    tick frequency in ticks-per-second (0 → 0)
 *
 * Returns floor(ticks * 1000 / hz), saturating at UINT64_MAX.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_tick_to_ms_u(uint64_t u64Ticks, uint64_t u64Hz)
{
	(void)NULL;
	return b4607_to_ms(u64Ticks, u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_to_ms_u(uint64_t u64Ticks, uint64_t u64Hz)
    __attribute__((alias("gj_tick_to_ms_u")));
