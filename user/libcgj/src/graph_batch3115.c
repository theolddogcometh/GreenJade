/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3115: uint64_t branchless max via mask (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_max_mask_u(uint64_t a, uint64_t b);
 *     - Larger of a and b, selected with an all-ones/zero compare mask
 *       (no ternary on the return path). Equal → b.
 *   uint64_t __gj_u64_max_mask_u  (alias)
 *   __libcgj_batch3115_marker = "libcgj-batch3115"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120). Distinct from
 * gj_u64_max_u / gj_u64_max_arr — unique gj_u64_max_mask_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3115_marker[] = "libcgj-batch3115";

/* ---- freestanding helpers ---------------------------------------------- */

/* max(a,b) via mask: mask = all-ones iff a > b. */
static uint64_t
b3115_max_mask(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Mask;

	u64Mask = 0ull - (uint64_t)(u64A > u64B);
	return (u64A & u64Mask) | (u64B & ~u64Mask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_max_mask_u - branchless greater of two uint64_t values.
 *
 * a, b: operands
 * Returns the larger value; when equal, returns b.
 */
uint64_t
gj_u64_max_mask_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3115_max_mask(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_max_mask_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_max_mask_u")));
