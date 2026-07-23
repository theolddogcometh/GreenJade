/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6783: inotify event-mask bit set.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_mask_bit_set_6783(uint32_t mask, uint32_t bit);
 *     - Return mask with bit set when bit < 32; else return mask
 *       unchanged (out-of-range is a no-op).
 *   uint32_t __gj_inotify_mask_bit_set_6783  (alias)
 *   __libcgj_batch6783_marker = "libcgj-batch6783"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs).
 * Unique gj_inotify_mask_bit_set_6783 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6783_marker[] = "libcgj-batch6783";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6783_set(uint32_t u32Mask, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return u32Mask;
	}
	return u32Mask | (1u << u32Bit);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_mask_bit_set_6783 - set one bit in an inotify event mask.
 *
 * mask: soft inotify event bitmask
 * bit:  bit index (0 = LSB); bit >= 32 leaves mask unchanged
 *
 * Returns updated mask. Soft pure-data op; no inotify_add_watch.
 */
uint32_t
gj_inotify_mask_bit_set_6783(uint32_t u32Mask, uint32_t u32Bit)
{
	(void)NULL;
	return b6783_set(u32Mask, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_mask_bit_set_6783(uint32_t u32Mask, uint32_t u32Bit)
    __attribute__((alias("gj_inotify_mask_bit_set_6783")));
