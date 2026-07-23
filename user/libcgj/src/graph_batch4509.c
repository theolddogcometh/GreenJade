/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4509: bitwise mask-merge of two uint32_t words.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mask_merge_u(uint32_t a, uint32_t b, uint32_t mask);
 *     - Return (a & mask) | (b & ~mask). Where mask is 1, take a; where 0,
 *       take b. Classic bit-blend / select-per-bit.
 *   uint32_t __gj_u32_mask_merge_u  (alias)
 *   __libcgj_batch4509_marker = "libcgj-batch4509"
 *
 * Exclusive continuum CREATE-ONLY (4501-4510). Distinct from
 * gj_u32_select_u (batch4425) which is whole-value select by nonzero mask —
 * unique gj_u32_mask_merge_u per-bit blend only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4509_marker[] = "libcgj-batch4509";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4509_mask_merge(uint32_t uA, uint32_t uB, uint32_t uMask)
{
	return (uA & uMask) | (uB & ~uMask);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mask_merge_u - per-bit blend of a and b controlled by mask.
 *
 * a: preferred bits where mask is 1
 * b: preferred bits where mask is 0
 * mask: bit selector (1 -> a, 0 -> b)
 *
 * Returns (a & mask) | (b & ~mask). Example:
 * gj_u32_mask_merge_u(0xF0F0u, 0x0F0Fu, 0xFF00u) -> 0xF00Fu.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_mask_merge_u(uint32_t uA, uint32_t uB, uint32_t uMask)
{
	(void)NULL;
	return b4509_mask_merge(uA, uB, uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mask_merge_u(uint32_t uA, uint32_t uB, uint32_t uMask)
    __attribute__((alias("gj_u32_mask_merge_u")));
