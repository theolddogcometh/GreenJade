/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3871: saturating uint64_t → int64_t cast (_u).
 *
 * Surface (unique symbols):
 *   int64_t gj_u64_to_i64_sat_u(uint64_t x);
 *     - If x <= (uint64_t)INT64_MAX, return (int64_t)x; else INT64_MAX.
 *       Never produces a negative result (unsigned input domain).
 *   int64_t __gj_u64_to_i64_sat_u  (alias)
 *   __libcgj_batch3871_marker = "libcgj-batch3871"
 *
 * Exclusive continuum CREATE-ONLY (3871-3880: u64_to_i64_sat_u,
 * str_is_ident_start_u, str_is_ident_cont_u, str_is_ident_u,
 * str_is_keyword_u, str_is_number_u, str_is_hex_u, str_is_ws_only_u,
 * str_trim_both_u, batch_id_3880). Distinct from
 * gj_u64_clamped_to_i64 (batch1738) — unique gj_u64_to_i64_sat_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3871_marker[] = "libcgj-batch3871";

/* ---- freestanding helpers ---------------------------------------------- */

/* INT64_MAX as uint64_t for range compare. */
static uint64_t
b3871_i64_max_u(void)
{
	return (uint64_t)INT64_MAX;
}

static int64_t
b3871_sat(uint64_t u64X)
{
	if (u64X > b3871_i64_max_u()) {
		return INT64_MAX;
	}
	return (int64_t)u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_to_i64_sat_u - map u64 into [0, INT64_MAX] as int64_t.
 *
 * Values above INT64_MAX saturate to INT64_MAX (no wrap / UB).
 * Does not call libc. No parent wires.
 */
int64_t
gj_u64_to_i64_sat_u(uint64_t u64X)
{
	(void)NULL;
	return b3871_sat(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_u64_to_i64_sat_u(uint64_t u64X)
    __attribute__((alias("gj_u64_to_i64_sat_u")));
