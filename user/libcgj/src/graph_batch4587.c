/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4587: mask a uint32_t flag word (value AND).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flags_mask_u(uint32_t f, uint32_t mask);
 *     - Return f & mask. Pure value transform; isolates only the bits
 *       present in mask. mask 0 → 0; mask all-ones leaves f unchanged.
 *   uint32_t __gj_flags_mask_u  (alias)
 *   __libcgj_batch4587_marker = "libcgj-batch4587"
 *
 * Exclusive continuum CREATE-ONLY (4581-4590). Distinct from
 * gj_flags_clear_u (batch4582, AND-not) and gj_ct_eq_mask_u64
 * (batch1513) — unique gj_flags_mask_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4587_marker[] = "libcgj-batch4587";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4587_mask(uint32_t u32F, uint32_t u32Mask)
{
	return u32F & u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flags_mask_u - keep only the bits of f that are set in mask.
 *
 * f:    flag word
 * mask: retention mask
 *
 * Returns f & mask. Self-contained; no parent wires.
 */
uint32_t
gj_flags_mask_u(uint32_t u32F, uint32_t u32Mask)
{
	(void)NULL;
	return b4587_mask(u32F, u32Mask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flags_mask_u(uint32_t u32F, uint32_t u32Mask)
    __attribute__((alias("gj_flags_mask_u")));
