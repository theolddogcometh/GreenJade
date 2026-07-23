/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6782: inotify event-mask bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_inotify_mask_bit_test_6782(uint32_t mask, uint32_t bit);
 *     - Return 1 if bit < 32 and (mask & (1u << bit)) != 0; else 0.
 *   uint32_t __gj_inotify_mask_bit_test_6782  (alias)
 *   __libcgj_batch6782_marker = "libcgj-batch6782"
 *
 * Exclusive continuum CREATE-ONLY (6781-6790: inotify event mask stubs).
 * Unique gj_inotify_mask_bit_test_6782 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6782_marker[] = "libcgj-batch6782";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6782_test(uint32_t u32Mask, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return 0u;
	}
	return ((u32Mask >> u32Bit) & 1u) != 0u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inotify_mask_bit_test_6782 - test one bit in an inotify event mask.
 *
 * mask: soft inotify event bitmask
 * bit:  bit index (0 = LSB / IN_ACCESS-style); bit >= 32 yields 0
 *
 * Returns 1 if set, else 0. Soft pure-data bit test; no inotify fd.
 */
uint32_t
gj_inotify_mask_bit_test_6782(uint32_t u32Mask, uint32_t u32Bit)
{
	(void)NULL;
	return b6782_test(u32Mask, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_inotify_mask_bit_test_6782(uint32_t u32Mask, uint32_t u32Bit)
    __attribute__((alias("gj_inotify_mask_bit_test_6782")));
