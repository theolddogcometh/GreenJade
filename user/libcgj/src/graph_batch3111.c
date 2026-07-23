/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3111: uint64_t greater-than mask (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_gt_mask_u(uint64_t a, uint64_t b);
 *     - Returns all-bits-one (~0ull) if a > b (unsigned), else 0.
 *       Full-width mask suitable for AND/select chains.
 *   uint64_t __gj_u64_gt_mask_u  (alias)
 *   __libcgj_batch3111_marker = "libcgj-batch3111"
 *
 * Exclusive continuum CREATE-ONLY (3111-3120). Distinct from
 * gj_ct_eq_mask_u64 / gj_ct_lt_u64 — unique gj_u64_gt_mask_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3111_marker[] = "libcgj-batch3111";

/* ---- freestanding helpers ---------------------------------------------- */

/* All-ones if a > b, else 0. */
static uint64_t
b3111_gt_mask(uint64_t u64A, uint64_t u64B)
{
	return 0ull - (uint64_t)(u64A > u64B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_gt_mask_u - unsigned greater-than mask of two uint64_t values.
 *
 * a > b → 0xffffffffffffffffull; else 0.
 */
uint64_t
gj_u64_gt_mask_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3111_gt_mask(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_gt_mask_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_gt_mask_u")));
