/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3114: uint32_t branchless max via mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_max_mask_u(uint32_t a, uint32_t b);
 *     - Larger of a and b, selected with an all-ones/zero compare mask
 *       (no ternary on the return path). Equal → b.
 *   uint32_t __gj_u32_max_mask_u  (alias)
 *   __libcgj_batch3114_marker = "libcgj-batch3114"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120). Distinct from
 * gj_u32_max_u / gj_u32_max3_u — unique gj_u32_max_mask_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3114_marker[] = "libcgj-batch3114";

/* ---- freestanding helpers ---------------------------------------------- */

/* max(a,b) via mask: mask = all-ones iff a > b. */
static uint32_t
b3114_max_mask(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32Mask;

	u32Mask = 0u - (uint32_t)(u32A > u32B);
	return (u32A & u32Mask) | (u32B & ~u32Mask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_max_mask_u - branchless greater of two uint32_t values.
 *
 * a, b: operands
 * Returns the larger value; when equal, returns b.
 */
uint32_t
gj_u32_max_mask_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3114_max_mask(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_max_mask_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_max_mask_u")));
