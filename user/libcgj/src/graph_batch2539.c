/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2539: saturating unsigned millisecond subtract.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_sub_sat_u(uint64_t a, uint64_t b);
 *     — a - b with saturation at 0 on underflow (no wrap).
 *   uint64_t __gj_ms_sub_sat_u  (alias)
 *   __libcgj_batch2539_marker = "libcgj-batch2539"
 *
 * Time exclusive pure-data wave (2531–2540). Millisecond-domain
 * saturating subtract. Distinct from gj_u64_sub_sat (batch2032) and
 * gj_i64_sub_sat_u (batch2042) — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2539_marker[] = "libcgj-batch2539";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a - b (clamp to 0; uint64_t milliseconds). */
static uint64_t
b2539_sub_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_sub_sat_u — unsigned millisecond subtract; clamp to 0 on underflow.
 *
 * a: millisecond minuend
 * b: millisecond subtrahend
 * Examples: (5,3) → 2; (3,5) → 0.
 */
uint64_t
gj_ms_sub_sat_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2539_sub_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_sub_sat_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ms_sub_sat_u")));
