/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2538: saturating unsigned millisecond add.
 *
 * Surface (unique symbols):
 *   uint64_t gj_ms_add_sat_u(uint64_t a, uint64_t b);
 *     — a + b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_ms_add_sat_u  (alias)
 *   __libcgj_batch2538_marker = "libcgj-batch2538"
 *
 * Time exclusive pure-data wave (2531–2540). Millisecond-domain
 * saturating add. Distinct from gj_u64_add_sat (batch2031) and
 * gj_i64_add_sat_u (batch2041) — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2538_marker[] = "libcgj-batch2538";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b (uint64_t milliseconds). */
static uint64_t
b2538_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ms_add_sat_u — unsigned millisecond add; clamp to UINT64_MAX on overflow.
 *
 * a, b: millisecond addends
 *
 * Detect overflow without wrapping: b > UINT64_MAX - a.
 * Examples: (1,2) → 3; (UINT64_MAX, 1) → UINT64_MAX.
 */
uint64_t
gj_ms_add_sat_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2538_add_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_ms_add_sat_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_ms_add_sat_u")));
