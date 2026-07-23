/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4602: saturating u64 tick subtract.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_sub_u(uint64_t a, uint64_t b);
 *     - a - b with saturation at 0 on underflow (no wrap).
 *       Tick-domain subtract for intervals / remaining budgets.
 *   uint64_t __gj_tick_sub_u  (alias)
 *   __libcgj_batch4602_marker = "libcgj-batch4602"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Distinct from
 * gj_u64_sub_sat (batch2032) / gj_u64_sat_sub (batch922) — unique
 * gj_tick_sub_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4602_marker[] = "libcgj-batch4602";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b (clamp to 0). */
static uint64_t
b4602_sub_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_sub_u - saturating unsigned tick subtraction.
 *
 * a: minuend (ticks)
 * b: subtrahend (ticks)
 *
 * Returns a - b, or 0 if a < b. Self-contained; no parent wires.
 */
uint64_t
gj_tick_sub_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4602_sub_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_sub_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_tick_sub_u")));
