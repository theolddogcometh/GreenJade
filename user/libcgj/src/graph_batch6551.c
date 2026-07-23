/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6551: VirtIO feature-bit word width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_feat_word_bits_6551(void);
 *     - Returns 32 (soft VirtIO feature mask word width in bits).
 *   uint32_t __gj_virtio_feat_word_bits_6551  (alias)
 *   __libcgj_batch6551_marker = "libcgj-batch6551"
 *
 * Exclusive continuum CREATE-ONLY (6551-6560: VirtIO feature bit stubs —
 * word_bits_6551, bit_test_6552, bit_set_6553, bit_clear_6554,
 * bit_toggle_6555, mask_pop_6556, mask_or_6557, mask_and/has_all_6558,
 * ring_baseline/ring_ok_6559, continuum + batch_id_6560). Unique
 * gj_virtio_feat_word_bits_6551 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6551_marker[] = "libcgj-batch6551";

/* Soft: one VirtIO feature select word is a uint32_t bitfield. */
#define B6551_WORD_BITS  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6551_word_bits(void)
{
	return B6551_WORD_BITS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_feat_word_bits_6551 - VirtIO feature-bit word width.
 *
 * Always returns 32. Soft pure-data layout tag for a single uint32_t
 * feature mask word (device/driver features[0] style). Does not probe
 * MMIO or PCI. No parent wires.
 */
uint32_t
gj_virtio_feat_word_bits_6551(void)
{
	(void)NULL;
	return b6551_word_bits();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virtio_feat_word_bits_6551(void)
    __attribute__((alias("gj_virtio_feat_word_bits_6551")));
