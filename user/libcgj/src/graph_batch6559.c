/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6559: VirtIO ring soft feature baseline.
 *
 * Surface (unique symbols):
 *   uint32_t gj_virtio_feat_ring_baseline_6559(void);
 *     - Returns soft ring feature baseline mask (0x30000000):
 *       bit 28 = RING_INDIRECT_DESC, bit 29 = RING_EVENT_IDX.
 *   uint32_t gj_virtio_feat_ring_ok_6559(uint32_t mask);
 *     - Return 1 if mask covers the ring baseline (has-all baseline).
 *   uint32_t __gj_virtio_feat_ring_baseline_6559  (alias)
 *   uint32_t __gj_virtio_feat_ring_ok_6559  (alias)
 *   __libcgj_batch6559_marker = "libcgj-batch6559"
 *
 * Exclusive continuum CREATE-ONLY (6551-6560: VirtIO feature bit stubs).
 * Unique gj_virtio_feat_*_6559 surfaces only; no multi-def. Distinct
 * from gj_virtio_*_u (batch5231-5239). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6559_marker[] = "libcgj-batch6559";

/*
 * Soft VirtIO ring baseline (transport features in features[0]):
 *   bit 28 = VIRTIO_F_RING_INDIRECT_DESC
 *   bit 29 = VIRTIO_F_RING_EVENT_IDX
 * Compile-time tag only; not a device probe.
 */
#define B6559_RING_BASELINE  0x30000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6559_baseline(void)
{
	return B6559_RING_BASELINE;
}

static uint32_t
b6559_ring_ok(uint32_t u32Mask)
{
	uint32_t u32Need = B6559_RING_BASELINE;

	return ((u32Mask & u32Need) == u32Need) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virtio_feat_ring_baseline_6559 - soft VirtIO ring baseline mask.
 *
 * Always returns 0x30000000. Soft pure-data product tag; does not query
 * VirtIO MMIO/PCI features. No parent wires.
 */
uint32_t
gj_virtio_feat_ring_baseline_6559(void)
{
	(void)NULL;
	return b6559_baseline();
}

/*
 * gj_virtio_feat_ring_ok_6559 - mask covers VirtIO ring baseline bits.
 *
 * mask: soft VirtIO feature bitmask
 *
 * Returns 1 if all baseline bits present; else 0.
 */
uint32_t
gj_virtio_feat_ring_ok_6559(uint32_t u32Mask)
{
	return b6559_ring_ok(u32Mask);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_virtio_feat_ring_baseline_6559(void)
    __attribute__((alias("gj_virtio_feat_ring_baseline_6559")));

uint32_t __gj_virtio_feat_ring_ok_6559(uint32_t u32Mask)
    __attribute__((alias("gj_virtio_feat_ring_ok_6559")));
