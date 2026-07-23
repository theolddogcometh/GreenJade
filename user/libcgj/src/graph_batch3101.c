/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3101: branch-free u32 cmov (unsigned form).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_cmov_u(uint32_t cond, uint32_t a, uint32_t b);
 *     - Branch-free: cond != 0 → a, else b. Pure unsigned API.
 *   uint32_t __gj_u32_cmov_u  (alias)
 *   __libcgj_batch3101_marker = "libcgj-batch3101"
 *
 * Exclusive continuum CREATE-ONLY (3101-3110): u32_cmov_u, u64_cmov_u,
 * u32_select_u, u64_select_u, u32_eq_mask_u, u64_eq_mask_u, u32_lt_mask_u,
 * u64_lt_mask_u, u32_gt_mask_u, batch_id_3110. Distinct from
 * gj_ct_cmov_u32 (batch1512) / gj_ct_select_u32 (batch1252) /
 * gj_select_u32 (batch120) — unique gj_u32_cmov_u surface only; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3101_marker[] = "libcgj-batch3101";

/* ---- freestanding helpers ---------------------------------------------- */

/* cond != 0 → a; else b. Branch-free on a/b. */
static uint32_t
b3101_cmov(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	uint32_t uC;
	uint32_t uMask;

	uC = (uint32_t)(uCond != 0u);
	uMask = 0u - uC;
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_cmov_u - branch-free select between two uint32_t values.
 *
 * cond != 0 → a; cond == 0 → b. Mask derived without a data-dependent
 * branch on a/b. No parent wires.
 */
uint32_t
gj_u32_cmov_u(uint32_t uCond, uint32_t uA, uint32_t uB)
{
	(void)NULL;
	return b3101_cmov(uCond, uA, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_cmov_u(uint32_t uCond, uint32_t uA, uint32_t uB)
    __attribute__((alias("gj_u32_cmov_u")));
