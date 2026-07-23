/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3136: saturating duration subtraction (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_duration_sub_sat_u(uint64_t a, uint64_t b);
 *     - Return a - b; if b > a return 0 (floor at zero, no wrap).
 *       Units are caller-defined (ns/us/ms).
 *   uint64_t __gj_duration_sub_sat_u  (alias)
 *   __libcgj_batch3136_marker = "libcgj-batch3136"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140). Distinct from
 * gj_time_sub_ns (batch1036) — unique duration_sub_sat_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3136_marker[] = "libcgj-batch3136";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3136_sub_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > u64A) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_duration_sub_sat_u - subtract durations with floor at zero.
 *
 * u64A: minuend
 * u64B: subtrahend
 *
 * Returns 0 when u64B > u64A instead of wrapping.
 */
uint64_t
gj_duration_sub_sat_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3136_sub_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_duration_sub_sat_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_duration_sub_sat_u")));
