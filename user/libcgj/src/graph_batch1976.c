/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1976: milliseconds to hardware ticks.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_to_ticks(uint64_t ms, uint64_t hz);
 *     — Return floor(ms * hz / 1000). hz == 0 → 0. Saturates at
 *       UINT64_MAX when the scaled product would overflow.
 *   uint64_t __gj_ms_to_ticks  (alias)
 *   __libcgj_batch1976_marker = "libcgj-batch1976"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Inverse shape of
 * gj_ticks_to_ms (batch1975). Pure integer; no global HZ. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1976_marker[] = "libcgj-batch1976";

/* Milliseconds scale factor for tick conversion. */
#define B1976_MS_SCALE  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1976_ms_to_ticks — floor(ms * hz / 1000) with saturation.
 *
 * Prefers (ms * hz) / 1000 when the product fits. For large factors,
 * rewrites as floor(ms/1000)*hz + floor((ms%1000)*hz/1000).
 */
static uint64_t
b1976_ms_to_ticks(uint64_t u64Ms, uint64_t u64Hz)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64Hz == 0ull) {
		return 0ull;
	}
	if (u64Ms <= (UINT64_MAX / u64Hz)) {
		return (u64Ms * u64Hz) / B1976_MS_SCALE;
	}
	u64Whole = u64Ms / B1976_MS_SCALE;
	u64Rem = u64Ms % B1976_MS_SCALE;
	if (u64Whole > (UINT64_MAX / u64Hz)) {
		return UINT64_MAX;
	}
	u64Whole *= u64Hz;
	/* rem < 1000; rem * hz overflows only if hz > UINT64_MAX/rem. */
	if (u64Rem == 0ull) {
		return u64Whole;
	}
	if (u64Hz <= (UINT64_MAX / u64Rem)) {
		u64Part = (u64Rem * u64Hz) / B1976_MS_SCALE;
	} else {
		/* hz huge: floor(rem * hz / 1000) via (hz/1000)*rem + ... */
		u64Part = (u64Hz / B1976_MS_SCALE) * u64Rem;
		u64Part += ((u64Hz % B1976_MS_SCALE) * u64Rem) / B1976_MS_SCALE;
	}
	if (u64Whole > (UINT64_MAX - u64Part)) {
		return UINT64_MAX;
	}
	return u64Whole + u64Part;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_to_ticks — convert milliseconds to timer ticks.
 *
 * ms: milliseconds
 * hz: tick frequency in ticks-per-second (0 → 0)
 * Returns floor(ms * hz / 1000), saturating at UINT64_MAX.
 */
uint64_t
gj_ms_to_ticks(uint64_t u64Ms, uint64_t u64Hz)
{
	(void)NULL;
	return b1976_ms_to_ticks(u64Ms, u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_to_ticks(uint64_t u64Ms, uint64_t u64Hz)
    __attribute__((alias("gj_ms_to_ticks")));
