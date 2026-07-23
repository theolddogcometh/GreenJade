/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5771: filesystem free inode remaining stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fs_free_inode_5771(uint32_t total, uint32_t used);
 *     - Saturating free-inode count: total - used when used <= total,
 *       else 0. Soft pure-integer FS free-inode probe; not a real
 *       statfs/inode bitmap walk.
 *   uint32_t gj_batch_id_5771(void);
 *     - Returns the compile-time graph batch number (5771).
 *   uint32_t __gj_fs_free_inode_5771  (alias)
 *   uint32_t __gj_batch_id_5771  (alias)
 *   __libcgj_batch5771_marker = "libcgj-batch5771"
 *
 * Exclusive continuum CREATE-ONLY (5771-5780: filesystem free
 * inode/block + btrfs/ext4 feature bit stubs). Unique *_5771 surfaces
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5771_marker[] = "libcgj-batch5771";

#define B5771_BATCH_ID  5771u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5771_free_inode(uint32_t u32Total, uint32_t u32Used)
{
	if (u32Used >= u32Total) {
		return 0u;
	}
	return u32Total - u32Used;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fs_free_inode_5771 - soft free-inode remaining from total/used.
 *
 * total: total inode capacity (soft counter)
 * used:  allocated inode count (soft counter)
 *
 * Returns total - used when used < total, else 0. Does not touch a
 * real superblock or inode bitmap. No parent wires.
 */
uint32_t
gj_fs_free_inode_5771(uint32_t total, uint32_t used)
{
	(void)NULL;
	return b5771_free_inode(total, used);
}

/*
 * gj_batch_id_5771 - report this TU's graph batch number.
 *
 * Always returns 5771.
 */
uint32_t
gj_batch_id_5771(void)
{
	return B5771_BATCH_ID;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fs_free_inode_5771(uint32_t total, uint32_t used)
    __attribute__((alias("gj_fs_free_inode_5771")));

uint32_t __gj_batch_id_5771(void)
    __attribute__((alias("gj_batch_id_5771")));
