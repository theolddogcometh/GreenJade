/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6058: thermal trip crossed predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_trip_crossed_u_6058(uint32_t temp_c, uint32_t trip_c);
 *     - Return 1 if measured temp_c is greater than or equal to the
 *       trip threshold trip_c (same unit, °C-style), else 0. Soft
 *       pure-data compare only; not a live sensor sample.
 *   uint32_t __gj_trip_crossed_u_6058  (alias)
 *   __libcgj_batch6058_marker = "libcgj-batch6058"
 *
 * Exclusive continuum CREATE-ONLY (6051-6060: thermal trip point
 * stubs). Unique gj_trip_crossed_u_6058 surface only; no multi-def.
 * Distinct from gj_pwr_throttle_pct_u (batch5338) and the 6051-6054
 * default trip temperature tags. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6058_marker[] = "libcgj-batch6058";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6058_crossed(uint32_t uTempC, uint32_t uTripC)
{
	if (uTempC >= uTripC) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_trip_crossed_u_6058 - 1 if temp_c >= trip_c.
 *
 * temp_c: measured temperature (°C-style)
 * trip_c: trip-point threshold (same unit)
 *
 * Returns 1 when the trip is at or crossed, else 0. Integer-only.
 * No parent wires.
 */
uint32_t
gj_trip_crossed_u_6058(uint32_t uTempC, uint32_t uTripC)
{
	(void)NULL;
	return b6058_crossed(uTempC, uTripC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_trip_crossed_u_6058(uint32_t uTempC, uint32_t uTripC)
    __attribute__((alias("gj_trip_crossed_u_6058")));
