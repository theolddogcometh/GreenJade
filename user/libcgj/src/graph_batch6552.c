/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6552: VirtIO feature-bit test.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_feat_bit_test_6552(uint32_t mask, uint32_t bit);
 *     - Return 1 if bit < 32 and (mask & (1u << bit)) != 0; else 0.
 *   uint32_t __gj_virtio_feat_bit_test_6552  (alias)
 *   __libcgj_batch6552_marker = "libcgj-batch6552"
 *
 * Exclusive continuum CREATE-ONLY (6551-6560: VirtIO feature bit stubs).
 * Unique gj_virtio_feat_bit_test_6552 surface only; no multi-def.
 * Distinct from gj_radv_feat_bit_test_6122 / gj_feature_test_bit.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6552_marker[] = "libcgj-batch6552";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6552_test(uint32_t u32Mask, uint32_t u32Bit)
{
	if (u32Bit >= 32u) {
		return 0u;
	}
	return ((u32Mask >> u32Bit) & 1u) != 0u ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_feat_bit_test_6552 - test one bit in a VirtIO feature mask.
 *
 * mask: soft VirtIO feature bitmask (one select word)
 * bit:  bit index (0 = LSB); bit >= 32 yields 0 (no shift UB)
 *
 * Returns 1 if set, else 0. Soft pure-data bit test; no VirtIO device.
 */
uint32_t
gj_virtio_feat_bit_test_6552(uint32_t u32Mask, uint32_t u32Bit)
{
	(void)NULL;
	return b6552_test(u32Mask, u32Bit);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_feat_bit_test_6552(uint32_t u32Mask, uint32_t u32Bit)
    __attribute__((alias("gj_virtio_feat_bit_test_6552")));
