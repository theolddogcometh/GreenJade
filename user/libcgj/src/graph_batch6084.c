/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6084: microSD sysfs block path prefix length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_microsd_sysfs_block_prefix_len_6084(void);
 *     - Returns 16 (byte length of soft "/sys/block/mmcblk" prefix).
 *   uint32_t __gj_microsd_sysfs_block_prefix_len_6084  (alias)
 *   __libcgj_batch6084_marker = "libcgj-batch6084"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_microsd_sysfs_block_prefix_len_6084 surface only;
 * no multi-def. Distinct from /dev/mmcblk prefix (batch6081). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6084_marker[] = "libcgj-batch6084";

/* Soft byte length of "/sys/block/mmcblk" (no trailing index). */
#define B6084_MICROSD_SYSFS_BLOCK_PREFIX_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6084_prefix_len(void)
{
	return B6084_MICROSD_SYSFS_BLOCK_PREFIX_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_microsd_sysfs_block_prefix_len_6084 - /sys/block/mmcblk prefix len.
 *
 * Always returns 16. Soft compile-time sysfs path stub for microSD /
 * eMMC block nodes; does not walk sysfs. No parent wires.
 */
uint32_t
gj_microsd_sysfs_block_prefix_len_6084(void)
{
	(void)NULL;
	return b6084_prefix_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_microsd_sysfs_block_prefix_len_6084(void)
    __attribute__((alias("gj_microsd_sysfs_block_prefix_len_6084")));
