/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5775: free blocks to bytes soft convert.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_free_bytes_lo_5775(uint32_t free_blocks,
 *                                    uint32_t block_size);
 *     - Low 32 bits of free_blocks * block_size (soft free-space
 *       bytes). Saturating block_size clamped to 0..1MiB soft max.
 *   uint32_t gj_fs_free_bytes_hi_5775(uint32_t free_blocks,
 *                                    uint32_t block_size);
 *     - High 32 bits of free_blocks * block_size.
 *   uint32_t gj_batch_id_5775(void);
 *   __ aliases for each public symbol
 *   __libcgj_batch5775_marker = "libcgj-batch5775"
 *
 * Exclusive continuum CREATE-ONLY (5771-5780: filesystem free
 * inode/block + btrfs/ext4 feature bit stubs). Unique *_5775 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5775_marker[] = "libcgj-batch5775";

#define B5775_BATCH_ID     5775u
#define B5775_BSZ_MAX      (1024u * 1024u)  /* soft 1 MiB block size cap */

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5775_clamp_bsz(uint32_t u32Bsz)
{
	if (u32Bsz > B5775_BSZ_MAX) {
		return B5775_BSZ_MAX;
	}
	return u32Bsz;
}

static uint64_t
b5775_free_bytes(uint32_t u32FreeBlocks, uint32_t u32BlockSize)
{
	uint32_t u32Bsz = b5775_clamp_bsz(u32BlockSize);

	return (uint64_t)u32FreeBlocks * (uint64_t)u32Bsz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_free_bytes_lo_5775 - low 32 bits of soft free bytes.
 *
 * free_blocks: free block count
 * block_size:  bytes per block (clamped to 0..1MiB)
 *
 * Returns (free_blocks * block_size) & 0xffffffff. No parent wires.
 */
uint32_t
gj_fs_free_bytes_lo_5775(uint32_t free_blocks, uint32_t block_size)
{
	(void)NULL;
	return (uint32_t)b5775_free_bytes(free_blocks, block_size);
}

/*
 * gj_fs_free_bytes_hi_5775 - high 32 bits of soft free bytes.
 *
 * free_blocks: free block count
 * block_size:  bytes per block (clamped to 0..1MiB)
 *
 * Returns (free_blocks * block_size) >> 32. No parent wires.
 */
uint32_t
gj_fs_free_bytes_hi_5775(uint32_t free_blocks, uint32_t block_size)
{
	return (uint32_t)(b5775_free_bytes(free_blocks, block_size) >> 32);
}

/*
 * gj_batch_id_5775 - report this TU's graph batch number.
 *
 * Always returns 5775.
 */
uint32_t
gj_batch_id_5775(void)
{
	return B5775_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fs_free_bytes_lo_5775(uint32_t free_blocks, uint32_t block_size)
    __attribute__((alias("gj_fs_free_bytes_lo_5775")));

uint32_t __gj_fs_free_bytes_hi_5775(uint32_t free_blocks, uint32_t block_size)
    __attribute__((alias("gj_fs_free_bytes_hi_5775")));

uint32_t __gj_batch_id_5775(void)
    __attribute__((alias("gj_batch_id_5775")));
