/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4213: uint64_t three-way minimum (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_min3_u2(uint64_t a, uint64_t b, uint64_t c);
 *     - Least of a, b, and c (unsigned compare). Pairwise fold:
 *       min(a, b) then min(that, c). On equality the later operand of
 *       each comparison is kept.
 *   uint64_t __gj_u64_min3_u2  (alias)
 *   __libcgj_batch4213_marker = "libcgj-batch4213"
 *
 * Exclusive continuum CREATE-ONLY (4211-4220: u32_min3_u, u32_max3_u,
 * u64_min3_u, u64_max3_u, u32_clamp_range_u, u64_clamp_range_u,
 * i32_clamp_range_s, i32_abs_s, i32_sign_s, batch_id_4220).
 * Distinct surface in this wave; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4213_marker[] = "libcgj-batch4213";

/* ---- freestanding helpers ---------------------------------------------- */

/* Least of three uint64 values. */
static uint64_t
b4213_min3(uint64_t uA, uint64_t uB, uint64_t uC)
{
	uint64_t uM;

	if (uA < uB) {
		uM = uA;
	} else {
		uM = uB;
	}
	if (uC < uM) {
		return uC;
	}
	return uM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_min3_u2 - return the least of three uint64_t values.
 *
 * a, b, c: candidates
 *
 * Pairwise reduction under unsigned compare. Does not call libc.
 */
uint64_t
gj_u64_min3_u2(uint64_t uA, uint64_t uB, uint64_t uC)
{
	(void)NULL;
	return b4213_min3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_min3_u2(uint64_t uA, uint64_t uB, uint64_t uC)
    __attribute__((alias("gj_u64_min3_u2")));
