/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1975: hardware ticks to milliseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ticks_to_ms(uint64_t ticks, uint64_t hz);
 *     — Return floor(ticks * 1000 / hz). hz == 0 → 0. Saturates at
 *       UINT64_MAX when the scaled quotient would overflow.
 *   uint64_t __gj_ticks_to_ms  (alias)
 *   __libcgj_batch1975_marker = "libcgj-batch1975"
 *
 * Time-duration exclusive pure-data wave (1971–1980). Pure integer
 * conversion; no global HZ. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1975_marker[] = "libcgj-batch1975";

/* Milliseconds scale factor for tick conversion. */
#define B1975_MS_SCALE  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b1975_ticks_to_ms — floor(ticks * 1000 / hz) with saturation.
 *
 * Prefers (ticks * 1000) / hz when the product fits. For large ticks,
 * rewrites as floor(ticks/hz)*1000 + floor((ticks%hz)*1000/hz).
 */
static uint64_t
b1975_ticks_to_ms(uint64_t u64Ticks, uint64_t u64Hz)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Part;

	if (u64Hz == 0ull) {
		return 0ull;
	}
	if (u64Ticks <= (UINT64_MAX / B1975_MS_SCALE)) {
		return (u64Ticks * B1975_MS_SCALE) / u64Hz;
	}
	u64Whole = u64Ticks / u64Hz;
	u64Rem = u64Ticks % u64Hz;
	if (u64Whole > (UINT64_MAX / B1975_MS_SCALE)) {
		return UINT64_MAX;
	}
	u64Whole *= B1975_MS_SCALE;
	if (u64Rem <= (UINT64_MAX / B1975_MS_SCALE)) {
		u64Part = (u64Rem * B1975_MS_SCALE) / u64Hz;
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
 * gj_ticks_to_ms — convert timer ticks to milliseconds.
 *
 * ticks: elapsed hardware/soft timer ticks
 * hz:    tick frequency in ticks-per-second (0 → 0)
 * Returns floor(ticks * 1000 / hz), saturating at UINT64_MAX.
 */
uint64_t
gj_ticks_to_ms(uint64_t u64Ticks, uint64_t u64Hz)
{
	(void)NULL;
	return b1975_ticks_to_ms(u64Ticks, u64Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ticks_to_ms(uint64_t u64Ticks, uint64_t u64Hz)
    __attribute__((alias("gj_ticks_to_ms")));
