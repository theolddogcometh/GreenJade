/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5780: MILESTONE 5780 + free summary pack.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_free_summary_pack_5780(uint32_t free_inodes,
 *                                        uint32_t free_blocks);
 *     - Pack soft free counts into a summary word:
 *         bits 0..15  = free_inodes clamped to 0..0xffff
 *         bits 16..31 = free_blocks clamped to 0..0xffff
 *   uint32_t gj_fs_free_summary_inodes_5780(uint32_t packed);
 *   uint32_t gj_fs_free_summary_blocks_5780(uint32_t packed);
 *   uint32_t gj_batch_id_5780(void);
 *     - Returns the compile-time graph batch number (5780).
 *   uint32_t gj_graph_milestone_5780(void);
 *     - Returns the graph milestone revision (5780).
 *   uint32_t gj_fs_feature_wave_ready_5780(void);
 *     - Returns 1 (FS free/feature continuum ready for wave 5780).
 *   __ aliases for each public symbol
 *   __libcgj_batch5780_marker = "libcgj-batch5780"
 *
 * MILESTONE 5780 for exclusive continuum CREATE-ONLY wave
 * (batches 5771-5779: free_inode_5771, free_block_5772,
 * free_inode_pct_5773, free_block_pct_5774, free_bytes_5775,
 * ext4_feature_mask/has_5776, ext4_feature_pack_5777,
 * btrfs_feature_mask/has_5778, btrfs_feature_pack_5779). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5780_marker[] = "libcgj-batch5780";

#define B5780_BATCH_ID     5780u
#define B5780_WAVE_READY   1u
#define B5780_NIB_MAX      0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5780_clamp16(uint32_t u32V)
{
	if (u32V > B5780_NIB_MAX) {
		return B5780_NIB_MAX;
	}
	return u32V;
}

static uint32_t
b5780_pack(uint32_t u32Inodes, uint32_t u32Blocks)
{
	return b5780_clamp16(u32Inodes) | (b5780_clamp16(u32Blocks) << 16);
}

static uint32_t
b5780_id(void)
{
	return B5780_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_free_summary_pack_5780 - pack soft free inode/block counts.
 *
 * free_inodes: free inode count (clamped to 16 bits)
 * free_blocks: free block count (clamped to 16 bits)
 *
 * Returns packed summary word. Soft pure-data only. No parent wires.
 */
uint32_t
gj_fs_free_summary_pack_5780(uint32_t free_inodes, uint32_t free_blocks)
{
	(void)NULL;
	return b5780_pack(free_inodes, free_blocks);
}

uint32_t
gj_fs_free_summary_inodes_5780(uint32_t packed)
{
	return packed & B5780_NIB_MAX;
}

uint32_t
gj_fs_free_summary_blocks_5780(uint32_t packed)
{
	return (packed >> 16) & B5780_NIB_MAX;
}

/*
 * gj_batch_id_5780 - report this TU's graph batch number.
 *
 * Always returns 5780.
 */
uint32_t
gj_batch_id_5780(void)
{
	return b5780_id();
}

/*
 * gj_graph_milestone_5780 - report this TU's graph milestone revision.
 *
 * Always returns 5780 (MILESTONE 5780). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link
 * time. No parent wires.
 */
uint32_t
gj_graph_milestone_5780(void)
{
	return b5780_id();
}

/*
 * gj_fs_feature_wave_ready_5780 - FS free/feature continuum ready lamp.
 *
 * Always returns 1 (ready). Soft pure-data only. No parent wires.
 */
uint32_t
gj_fs_feature_wave_ready_5780(void)
{
	return B5780_WAVE_READY;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fs_free_summary_pack_5780(uint32_t free_inodes,
    uint32_t free_blocks)
    __attribute__((alias("gj_fs_free_summary_pack_5780")));

uint32_t __gj_fs_free_summary_inodes_5780(uint32_t packed)
    __attribute__((alias("gj_fs_free_summary_inodes_5780")));

uint32_t __gj_fs_free_summary_blocks_5780(uint32_t packed)
    __attribute__((alias("gj_fs_free_summary_blocks_5780")));

uint32_t __gj_batch_id_5780(void)
    __attribute__((alias("gj_batch_id_5780")));

uint32_t __gj_graph_milestone_5780(void)
    __attribute__((alias("gj_graph_milestone_5780")));

uint32_t __gj_fs_feature_wave_ready_5780(void)
    __attribute__((alias("gj_fs_feature_wave_ready_5780")));
