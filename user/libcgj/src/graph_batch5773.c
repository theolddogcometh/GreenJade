/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5773: filesystem free-inode percent stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_free_inode_pct_5773(uint32_t total, uint32_t free_inodes);
 *     - Free-inode percentage 0..100: (free * 100) / total when
 *       free <= total and total != 0; clamps free to total; 0 if
 *       total is 0. Soft pure-integer; not a real f_ffree probe.
 *   uint32_t gj_batch_id_5773(void);
 *     - Returns the compile-time graph batch number (5773).
 *   uint32_t __gj_fs_free_inode_pct_5773  (alias)
 *   uint32_t __gj_batch_id_5773  (alias)
 *   __libcgj_batch5773_marker = "libcgj-batch5773"
 *
 * Exclusive continuum CREATE-ONLY (5771-5780: filesystem free
 * inode/block + btrfs/ext4 feature bit stubs). Unique *_5773 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5773_marker[] = "libcgj-batch5773";

#define B5773_BATCH_ID  5773u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5773_free_inode_pct(uint32_t u32Total, uint32_t u32Free)
{
	uint32_t u32Clamped;

	if (u32Total == 0u) {
		return 0u;
	}
	u32Clamped = (u32Free > u32Total) ? u32Total : u32Free;
	/* free*100 fits in uint32 when free <= total and total <= ~42M;
	 * for larger totals use 64-bit-free multiply via uint64_t. */
	return (uint32_t)(((uint64_t)u32Clamped * 100ull) / (uint64_t)u32Total);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_free_inode_pct_5773 - soft free-inode percentage 0..100.
 *
 * total:       total inode capacity
 * free_inodes: free inode count (clamped to total)
 *
 * Returns 0..100. Integer-only; does not call statfs. No parent wires.
 */
uint32_t
gj_fs_free_inode_pct_5773(uint32_t total, uint32_t free_inodes)
{
	(void)NULL;
	return b5773_free_inode_pct(total, free_inodes);
}

/*
 * gj_batch_id_5773 - report this TU's graph batch number.
 *
 * Always returns 5773.
 */
uint32_t
gj_batch_id_5773(void)
{
	return B5773_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fs_free_inode_pct_5773(uint32_t total, uint32_t free_inodes)
    __attribute__((alias("gj_fs_free_inode_pct_5773")));

uint32_t __gj_batch_id_5773(void)
    __attribute__((alias("gj_batch_id_5773")));
