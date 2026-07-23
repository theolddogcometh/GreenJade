/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6127: Mesa RADV feature-mask OR merge.
 *
 * Surface (unique symbols):
 *   uint32_t gj_radv_feat_mask_or_6127(uint32_t a, uint32_t b);
 *     - Return a | b (soft merge of two RADV feature words).
 *   uint32_t __gj_radv_feat_mask_or_6127  (alias)
 *   __libcgj_batch6127_marker = "libcgj-batch6127"
 *
 * Exclusive CREATE-ONLY (6121-6130: mesa radv feature bit stubs).
 * Unique gj_radv_feat_mask_or_6127 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6127_marker[] = "libcgj-batch6127";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6127_or(uint32_t u32A, uint32_t u32B)
{
	return u32A | u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_radv_feat_mask_or_6127 - bitwise OR of two RADV feature masks.
 *
 * a, b: soft RADV feature bitmasks
 *
 * Returns a | b. Soft pure-data merge; no Vulkan ICD.
 */
uint32_t
gj_radv_feat_mask_or_6127(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b6127_or(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_radv_feat_mask_or_6127(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_radv_feat_mask_or_6127")));
