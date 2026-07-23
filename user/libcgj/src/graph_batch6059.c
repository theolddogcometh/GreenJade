/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6059: thermal trip hysteresis apply (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_hyst_apply_u_6059(uint32_t trip_c, uint32_t hyst_c);
 *     - Return trip_c - hyst_c when trip_c > hyst_c, else 0. Soft
 *       clear/re-arm temperature after a trip with hysteresis. Pure
 *       integer arithmetic; not a live thermal governor.
 *   uint32_t __gj_trip_hyst_apply_u_6059  (alias)
 *   __libcgj_batch6059_marker = "libcgj-batch6059"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_hyst_apply_u_6059 surface only; no multi-def.
 * Distinct from gj_trip_crossed_u_6058 (threshold compare).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6059_marker[] = "libcgj-batch6059";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Clear/re-arm temperature: max(0, trip_c - hyst_c) via unsigned
 * saturating subtract (underflow → 0).
 */
static uint32_t
b6059_hyst_apply(uint32_t uTripC, uint32_t uHystC)
{
	if (uTripC <= uHystC) {
		return 0u;
	}
	return uTripC - uHystC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_hyst_apply_u_6059 - trip clear temperature with hysteresis.
 *
 * trip_c: trip-point threshold (°C-style)
 * hyst_c: hysteresis margin (same unit)
 *
 * Returns trip_c - hyst_c when trip_c > hyst_c, else 0. Integer-only.
 * No parent wires.
 */
uint32_t
gj_trip_hyst_apply_u_6059(uint32_t uTripC, uint32_t uHystC)
{
	(void)NULL;
	return b6059_hyst_apply(uTripC, uHystC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_hyst_apply_u_6059(uint32_t uTripC, uint32_t uHystC)
    __attribute__((alias("gj_trip_hyst_apply_u_6059")));
