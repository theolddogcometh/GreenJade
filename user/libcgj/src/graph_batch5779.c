/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5779: btrfs soft feature-bit pack / extract.
 *
 * Surface (unique symbols):
 *   uint32_t gj_btrfs_feature_pack_5779(uint32_t mixed_bg, uint32_t compress,
 *                                      uint32_t raid56, uint32_t free_meta);
 *     - Pack soft feature flags:
 *         bit0 = mixed_bg, bit1 = compress, bit2 = raid56,
 *         bit3 = free_space_tree (free_meta bit0),
 *         bit4 = skinny_metadata (free_meta bit1).
 *   uint32_t gj_btrfs_feature_mixed_bg_5779(uint32_t packed);
 *   uint32_t gj_btrfs_feature_compress_5779(uint32_t packed);
 *   uint32_t gj_btrfs_feature_raid56_5779(uint32_t packed);
 *   uint32_t gj_batch_id_5779(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5779_marker = "libcgj-batch5779"
 *
 * Exclusive continuum CREATE-ONLY (5771-5780). Unique *_5779 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5779_marker[] = "libcgj-batch5779";

#define B5779_BATCH_ID  5779u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5779_bit01(uint32_t u32V)
{
	return (u32V != 0u) ? 1u : 0u;
}

static uint32_t
b5779_pack(uint32_t u32Mixed, uint32_t u32Compress, uint32_t u32Raid56,
    uint32_t u32FreeMeta)
{
	uint32_t u32P;

	u32P = b5779_bit01(u32Mixed)
	    | (b5779_bit01(u32Compress) << 1)
	    | (b5779_bit01(u32Raid56) << 2)
	    | ((u32FreeMeta & 1u) << 3)         /* free_space_tree */
	    | (((u32FreeMeta >> 1) & 1u) << 4); /* skinny_metadata */
	return u32P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btrfs_feature_pack_5779 - pack soft btrfs feature flags.
 *
 * mixed_bg/compress/raid56: non-zero => set corresponding bit
 * free_meta: bit0 => free_space_tree, bit1 => skinny_metadata
 *
 * Returns packed 5-bit soft feature word.
 */
uint32_t
gj_btrfs_feature_pack_5779(uint32_t mixed_bg, uint32_t compress,
    uint32_t raid56, uint32_t free_meta)
{
	(void)NULL;
	return b5779_pack(mixed_bg, compress, raid56, free_meta);
}

uint32_t
gj_btrfs_feature_mixed_bg_5779(uint32_t packed)
{
	return packed & 1u;
}

uint32_t
gj_btrfs_feature_compress_5779(uint32_t packed)
{
	return (packed >> 1) & 1u;
}

uint32_t
gj_btrfs_feature_raid56_5779(uint32_t packed)
{
	return (packed >> 2) & 1u;
}

/*
 * gj_batch_id_5779 - report this TU's graph batch number.
 *
 * Always returns 5779.
 */
uint32_t
gj_batch_id_5779(void)
{
	return B5779_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_btrfs_feature_pack_5779(uint32_t mixed_bg, uint32_t compress,
    uint32_t raid56, uint32_t free_meta)
    __attribute__((alias("gj_btrfs_feature_pack_5779")));

uint32_t __gj_btrfs_feature_mixed_bg_5779(uint32_t packed)
    __attribute__((alias("gj_btrfs_feature_mixed_bg_5779")));

uint32_t __gj_btrfs_feature_compress_5779(uint32_t packed)
    __attribute__((alias("gj_btrfs_feature_compress_5779")));

uint32_t __gj_btrfs_feature_raid56_5779(uint32_t packed)
    __attribute__((alias("gj_btrfs_feature_raid56_5779")));

uint32_t __gj_batch_id_5779(void)
    __attribute__((alias("gj_batch_id_5779")));
