/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2895: product bar3 all-slots soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_mask_all_u(uint32_t mask);
 *     - Return 1 if mask has all four product bar3 readiness bits set
 *       (steam/deck/top50/install: bits 0..3), i.e.
 *       (mask & 0xf) == 0xf. Else 0. Soft pure-data completeness gate
 *       in unsigned 0/1 form.
 *   uint32_t __gj_bar3_mask_all_u  (alias)
 *   __libcgj_batch2895_marker = "libcgj-batch2895"
 *
 * Milestone 2900 exclusive product helpers (2891-2900). Unique
 * gj_bar3_mask_all_u surface only; no multi-def. Distinct from
 * gj_bar3_all_ready_p (batch2675 int), gj_bar3_ready_hint (batch2299
 * three-bit 0x7), and gj_product_bar3_mask (batch2597 constant).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2895_marker[] = "libcgj-batch2895";

/* Four product bar3 readiness bits: steam/deck/top50/install (0..3). */
#define B2895_ALL_MASK  0xfu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if all four bar3 readiness bits are present in mask. */
static uint32_t
b2895_mask_all(uint32_t uMask)
{
	if ((uMask & B2895_ALL_MASK) == B2895_ALL_MASK) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_mask_all_u - soft check that bar3 readiness mask is complete.
 *
 * mask: product bar3 readiness bitmask (low four bits required)
 *
 * Returns 1 when (mask & 0xf) == 0xf, else 0. Extra high bits are
 * ignored. Covers install + steam + deck + top50. Does not call libc.
 */
uint32_t
gj_bar3_mask_all_u(uint32_t uMask)
{
	(void)NULL;
	return b2895_mask_all(uMask);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_mask_all_u(uint32_t uMask)
    __attribute__((alias("gj_bar3_mask_all_u")));
