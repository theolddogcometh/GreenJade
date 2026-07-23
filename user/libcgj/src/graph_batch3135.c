/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3135: saturating duration addition (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_duration_add_sat_u(uint64_t a, uint64_t b);
 *     - Return a + b, saturating at UINT64_MAX on overflow.
 *       Units are caller-defined (ns/us/ms).
 *   uint64_t __gj_duration_add_sat_u  (alias)
 *   __libcgj_batch3135_marker = "libcgj-batch3135"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_time_add_ns (batch1035) — unique duration_add_sat_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3135_marker[] = "libcgj-batch3135";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3135_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A > (UINT64_MAX - u64B)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_duration_add_sat_u - add two durations with saturation.
 *
 * u64A, u64B: duration values (same unit)
 *
 * If a + b would wrap past UINT64_MAX, returns UINT64_MAX.
 */
uint64_t
gj_duration_add_sat_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3135_add_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_duration_add_sat_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_duration_add_sat_u")));
