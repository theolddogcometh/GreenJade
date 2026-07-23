/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4212: uint32_t three-way maximum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_max3_u2(uint32_t a, uint32_t b, uint32_t c);
 *     - Greatest of a, b, and c (unsigned compare). Pairwise fold:
 *       max(a, b) then max(that, c). On equality the later operand of
 *       each comparison is kept.
 *   uint32_t __gj_u32_max3_u2  (alias)
 *   __libcgj_batch4212_marker = "libcgj-batch4212"
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

const char __libcgj_batch4212_marker[] = "libcgj-batch4212";

/* ---- freestanding helpers ---------------------------------------------- */

/* Greatest of three uint32 values. */
static uint32_t
b4212_max3(uint32_t uA, uint32_t uB, uint32_t uC)
{
	uint32_t uM;

	if (uA > uB) {
		uM = uA;
	} else {
		uM = uB;
	}
	if (uM > uC) {
		return uM;
	}
	return uC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_max3_u2 - return the greatest of three uint32_t values.
 *
 * a, b, c: candidates
 *
 * Pairwise reduction under unsigned compare. Does not call libc.
 */
uint32_t
gj_u32_max3_u2(uint32_t uA, uint32_t uB, uint32_t uC)
{
	(void)NULL;
	return b4212_max3(uA, uB, uC);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_max3_u2(uint32_t uA, uint32_t uB, uint32_t uC)
    __attribute__((alias("gj_u32_max3_u2")));
