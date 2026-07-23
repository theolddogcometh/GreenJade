/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4606: milliseconds to timer ticks.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_from_ms_u(uint64_t ms, uint64_t hz);
 *     - Return floor(ms * hz / 1000). hz == 0 → 0. Saturates at
 *       UINT64_MAX when the scaled product would overflow.
 *   uint64_t __gj_tick_from_ms_u  (alias)
 *   __libcgj_batch4606_marker = "libcgj-batch4606"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Distinct from
 * gj_ms_to_ticks (batch1976) — unique gj_tick_from_ms_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4606_marker[] = "libcgj-batch4606";

/* Milliseconds scale factor for tick conversion. */
#define B4606_MS_SCALE  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4606_from_ms — floor(ms * hz / 1000) with saturation.
 *
 * Prefers (ms * hz) / 1000 when the product fits. For large factors,
 * rewrites as floor(ms/1000)*hz + floor((ms%1000)*hz/1000).
 */
static uint64_t
b4606_from_ms(uint64_t u64Ms, uint64_t u64Hz)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64Hz == 0ull) {
		return 0ull;
	}
	if (u64Ms <= (UINT64_MAX / u64Hz)) {
		return (u64Ms * u64Hz) / B4606_MS_SCALE;
	}
	u64Whole = u64Ms / B4606_MS_SCALE;
	u64Rem = u64Ms % B4606_MS_SCALE;
	if (u64Whole > (UINT64_MAX / u64Hz)) {
		return UINT64_MAX;
	}
	u64Whole *= u64Hz;
	if (u64Rem == 0ull) {
		return u64Whole;
	}
	if (u64Hz <= (UINT64_MAX / u64Rem)) {
		u64Part = (u64Rem * u64Hz) / B4606_MS_SCALE;
	} else {
		/* hz huge: floor(rem * hz / 1000) via (hz/1000)*rem + ... */
		u64Part = (u64Hz / B4606_MS_SCALE) * u64Rem;
		u64Part += ((u64Hz % B4606_MS_SCALE) * u64Rem) / B4606_MS_SCALE;
	}
	if (u64Whole > (UINT64_MAX - u64Part)) {
		return UINT64_MAX;
	}
	return u64Whole + u64Part;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_from_ms_u - convert milliseconds to timer ticks.
 *
 * ms: milliseconds
 * hz: tick frequency in ticks-per-second (0 → 0)
 *
 * Returns floor(ms * hz / 1000), saturating at UINT64_MAX.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_tick_from_ms_u(uint64_t u64Ms, uint64_t u64Hz)
{
	(void)NULL;
	return b4606_from_ms(u64Ms, u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_from_ms_u(uint64_t u64Ms, uint64_t u64Hz)
    __attribute__((alias("gj_tick_from_ms_u")));
