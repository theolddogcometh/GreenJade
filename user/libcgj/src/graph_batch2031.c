/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2031: saturating uint64_t addition.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_add_sat(uint64_t a, uint64_t b);
 *     — a + b with saturation at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_add_sat  (alias)
 *   __libcgj_batch2031_marker = "libcgj-batch2031"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_sat_add_u64 (batch377) and gj_u64_sat_add (batch921) — different
 * public names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2031_marker[] = "libcgj-batch2031";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating a + b. */
static uint64_t
b2031_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_add_sat — unsigned add; clamp to UINT64_MAX if a + b overflows.
 *
 * a, b: addends
 *
 * Detect overflow without wrapping: b > UINT64_MAX - a.
 */
uint64_t
gj_u64_add_sat(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2031_add_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_add_sat(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_add_sat")));
