/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3131: milliseconds to microseconds (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_time_ms_to_us_u(uint64_t ms);
 *     - Return ms * 1000, saturating at UINT64_MAX on overflow.
 *   uint64_t __gj_time_ms_to_us_u  (alias)
 *   __libcgj_batch3131_marker = "libcgj-batch3131"
 *
 * Exclusive continuum CREATE-ONLY (3131-3140): time_ms_to_us_u,
 * time_us_to_ms_u, time_s_to_ms_u, time_ms_to_s_u, duration_add_sat_u,
 * duration_sub_sat_u, deadline_passed_u, timeout_remaining_u,
 * hz_to_period_ns_u, batch_id_3140. Distinct from gj_time_ms_to_us
 * (batch1033) — unique _u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3131_marker[] = "libcgj-batch3131";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating multiply by 1000. */
static uint64_t
b3131_ms_to_us(uint64_t u64Ms)
{
	if (u64Ms > (UINT64_MAX / 1000ull)) {
		return UINT64_MAX;
	}
	return u64Ms * 1000ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_time_ms_to_us_u - convert milliseconds to microseconds.
 *
 * u64Ms: milliseconds
 *
 * Multiplies by 1000 with saturation on overflow.
 */
uint64_t
gj_time_ms_to_us_u(uint64_t u64Ms)
{
	(void)NULL;
	return b3131_ms_to_us(u64Ms);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_time_ms_to_us_u(uint64_t u64Ms)
    __attribute__((alias("gj_time_ms_to_us_u")));
