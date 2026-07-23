/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4601: saturating u64 tick add.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_add_u(uint64_t a, uint64_t b);
 *     - a + b with saturation at UINT64_MAX on overflow (no wrap).
 *       Tick-domain add for timer intervals / absolute deadlines.
 *   uint64_t __gj_tick_add_u  (alias)
 *   __libcgj_batch4601_marker = "libcgj-batch4601"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610: tick_add_u, tick_sub_u,
 * tick_elapsed_u, tick_expired_u, tick_remaining_u, tick_from_ms_u,
 * tick_to_ms_u, tick_clamp_u, tick_is_zero_u, batch_id_4610).
 * Distinct from gj_u64_add_sat (batch2031) / gj_u64_sat_add (batch921)
 * — unique gj_tick_add_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4601_marker[] = "libcgj-batch4601";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b for tick quantities. */
static uint64_t
b4601_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tick_add_u - saturating unsigned tick addition.
 *
 * a, b: tick addends (same unit)
 *
 * Returns a + b, clamping to UINT64_MAX on overflow. Detect overflow
 * without wrapping: b > UINT64_MAX - a. Self-contained; no parent wires.
 */
uint64_t
gj_tick_add_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b4601_add_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_add_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_tick_add_u")));
