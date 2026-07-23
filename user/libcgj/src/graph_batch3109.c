/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3109: constant-time u32 greater-than mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_gt_mask_u(uint32_t a, uint32_t b);
 *     - Returns all-bits-one (~0u) if a > b (unsigned), else 0.
 *       Branch-free on a/b values.
 *   uint32_t __gj_u32_gt_mask_u  (alias)
 *   __libcgj_batch3109_marker = "libcgj-batch3109"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110). Distinct from
 * gj_u32_lt_mask_u (batch3107) — unique gj_u32_gt_mask_u surface only;
 * no multi-def. Implemented independently (no parent wires).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3109_marker[] = "libcgj-batch3109";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * a > b (unsigned) → ~0u; else 0.
 * Equivalent to b < a via 64-bit subtract borrow (no __int128).
 */
static uint32_t
b3109_gt_mask(uint32_t uA, uint32_t uB)
{
	uint32_t uGt;

	/* b < a borrow bit */
	uGt = (uint32_t)(((uint64_t)uB - (uint64_t)uA) >> 63);
	return 0u - uGt;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_gt_mask_u - constant-time unsigned a > b mask (32-bit).
 *
 * a > b → 0xffffffffu; else 0. No parent wires.
 */
uint32_t
gj_u32_gt_mask_u(uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b3109_gt_mask(uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_gt_mask_u(uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_gt_mask_u")));
