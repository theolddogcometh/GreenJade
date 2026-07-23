/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4806: event-loop interest mask membership test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ev_mask_has_u(uint32_t mask, uint32_t bit);
 *     - Return 1 when (mask & bit) is non-zero, else 0.
 *   uint32_t __gj_ev_mask_has_u  (alias)
 *   __libcgj_batch4806_marker = "libcgj-batch4806"
 *
 * Exclusive continuum CREATE-ONLY (4801-4810). Unique
 * gj_ev_mask_has_u surface only; no multi-def. Distinct from
 * gj_ev_mask_or_u (batch4807). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4806_marker[] = "libcgj-batch4806";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4806_mask_has(uint32_t u32Mask, uint32_t u32Bit)
{
	if ((u32Mask & u32Bit) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ev_mask_has_u - test whether an interest mask contains a bit.
 *
 * mask: combined interest/ready mask
 * bit:  single interest bit (or any non-zero test pattern)
 *
 * Returns 1 when (mask & bit) != 0, otherwise 0. No parent wires.
 */
uint32_t
gj_ev_mask_has_u(uint32_t u32Mask, uint32_t u32Bit)
{
	(void)NULL;
	return b4806_mask_has(u32Mask, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ev_mask_has_u(uint32_t u32Mask, uint32_t u32Bit)
    __attribute__((alias("gj_ev_mask_has_u")));
