/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5772: filesystem free block remaining stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_free_block_5772(uint32_t total, uint32_t used);
 *     - Saturating free-block count: total - used when used <= total,
 *       else 0. Soft pure-integer FS free-block probe; not a real
 *       block-bitmap or group descriptor walk.
 *   uint32_t gj_batch_id_5772(void);
 *     - Returns the compile-time graph batch number (5772).
 *   uint32_t __gj_fs_free_block_5772  (alias)
 *   uint32_t __gj_batch_id_5772  (alias)
 *   __libcgj_batch5772_marker = "libcgj-batch5772"
 *
 * Exclusive continuum CREATE-ONLY (5771-5780: filesystem free
 * inode/block + btrfs/ext4 feature bit stubs). Unique *_5772 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5772_marker[] = "libcgj-batch5772";

#define B5772_BATCH_ID  5772u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5772_free_block(uint32_t u32Total, uint32_t u32Used)
{
	if (u32Used >= u32Total) {
		return 0u;
	}
	return u32Total - u32Used;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_free_block_5772 - soft free-block remaining from total/used.
 *
 * total: total block capacity (soft counter)
 * used:  allocated block count (soft counter)
 *
 * Returns total - used when used < total, else 0. Does not touch a
 * real superblock or block bitmap. No parent wires.
 */
uint32_t
gj_fs_free_block_5772(uint32_t total, uint32_t used)
{
	(void)NULL;
	return b5772_free_block(total, used);
}

/*
 * gj_batch_id_5772 - report this TU's graph batch number.
 *
 * Always returns 5772.
 */
uint32_t
gj_batch_id_5772(void)
{
	return B5772_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fs_free_block_5772(uint32_t total, uint32_t used)
    __attribute__((alias("gj_fs_free_block_5772")));

uint32_t __gj_batch_id_5772(void)
    __attribute__((alias("gj_batch_id_5772")));
