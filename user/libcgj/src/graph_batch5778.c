/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5778: btrfs soft feature-bit mask / has stubs.
 *
 * Surface (unique symbols):
 *   uint32_t gj_btrfs_feature_mask_5778(void);
 *     - Soft known-feature mask (mixed_bg|compress|raid56|free_space_tree|
 *       skinny_metadata).
 *   uint32_t gj_btrfs_feature_has_5778(uint32_t flags, uint32_t bit);
 *     - Return 1 when flags has the bit set, else 0.
 *   uint32_t gj_btrfs_feature_any_5778(uint32_t flags);
 *     - Return 1 when flags intersects the soft known mask, else 0.
 *   uint32_t gj_batch_id_5778(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5778_marker = "libcgj-batch5778"
 *
 * Soft layout only (not a real super_compat_ro / incompat read).
 * Exclusive continuum CREATE-ONLY (5771-5780). Unique *_5778 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5778_marker[] = "libcgj-batch5778";

#define B5778_BATCH_ID  5778u

/* Soft btrfs-like feature bits (layout probe constants only). */
#define B5778_MIXED_BG         0x01u
#define B5778_COMPRESS         0x02u
#define B5778_RAID56           0x04u
#define B5778_FREE_SPACE_TREE  0x08u
#define B5778_SKINNY_META      0x10u
#define B5778_MASK \
	(B5778_MIXED_BG | B5778_COMPRESS | B5778_RAID56 | \
	 B5778_FREE_SPACE_TREE | B5778_SKINNY_META)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5778_mask(void)
{
	return B5778_MASK;
}

static uint32_t
b5778_has(uint32_t u32Flags, uint32_t u32Bit)
{
	return ((u32Flags & u32Bit) != 0u) ? 1u : 0u;
}

static uint32_t
b5778_any(uint32_t u32Flags)
{
	return ((u32Flags & B5778_MASK) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btrfs_feature_mask_5778 - soft known btrfs feature mask.
 *
 * Always returns 0x1f (mixed_bg|compress|raid56|free_space_tree|
 * skinny_metadata). Pure product-path constant; not a superblock parse.
 */
uint32_t
gj_btrfs_feature_mask_5778(void)
{
	(void)NULL;
	return b5778_mask();
}

/*
 * gj_btrfs_feature_has_5778 - test one soft feature bit in flags.
 *
 * flags: candidate feature word
 * bit:   feature bit to test (e.g. 0x08 free_space_tree)
 *
 * Returns 1 if (flags & bit) != 0, else 0.
 */
uint32_t
gj_btrfs_feature_has_5778(uint32_t flags, uint32_t bit)
{
	return b5778_has(flags, bit);
}

/*
 * gj_btrfs_feature_any_5778 - any known soft feature bit set?
 *
 * flags: candidate feature word
 *
 * Returns 1 if flags intersects the soft known mask, else 0.
 */
uint32_t
gj_btrfs_feature_any_5778(uint32_t flags)
{
	return b5778_any(flags);
}

/*
 * gj_batch_id_5778 - report this TU's graph batch number.
 *
 * Always returns 5778.
 */
uint32_t
gj_batch_id_5778(void)
{
	return B5778_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_btrfs_feature_mask_5778(void)
    __attribute__((alias("gj_btrfs_feature_mask_5778")));

uint32_t __gj_btrfs_feature_has_5778(uint32_t flags, uint32_t bit)
    __attribute__((alias("gj_btrfs_feature_has_5778")));

uint32_t __gj_btrfs_feature_any_5778(uint32_t flags)
    __attribute__((alias("gj_btrfs_feature_any_5778")));

uint32_t __gj_batch_id_5778(void)
    __attribute__((alias("gj_batch_id_5778")));
