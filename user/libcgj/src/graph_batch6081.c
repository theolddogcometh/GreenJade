/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6081: SD / microSD /dev/mmcblk path prefix length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_dev_mmcblk_prefix_len_6081(void);
 *     - Returns 11 (byte length of soft "/dev/mmcblk" device-path prefix).
 *   uint32_t __gj_sd_dev_mmcblk_prefix_len_6081  (alias)
 *   __libcgj_batch6081_marker = "libcgj-batch6081"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs —
 * sd_dev_mmcblk_prefix_len_6081, sd_mmcblk_index_ok_6082,
 * sd_part_index_ok_6083, microsd_sysfs_block_prefix_len_6084,
 * sd_mount_path_max_6085, sd_path_join_need_6086,
 * microsd_by_id_prefix_len_6087, sd_path_is_abs_hint_6088,
 * sd_path_stub_ready_6089, continuum + batch_id_6090). Unique surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6081_marker[] = "libcgj-batch6081";

/* Soft byte length of "/dev/mmcblk" (no trailing index or partition). */
#define B6081_SD_DEV_MMCBLK_PREFIX_LEN  11u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6081_prefix_len(void)
{
	return B6081_SD_DEV_MMCBLK_PREFIX_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_dev_mmcblk_prefix_len_6081 - /dev/mmcblk path prefix length.
 *
 * Always returns 11. Soft compile-time path stub for SD / eMMC block
 * device names; does not open or probe devices. No parent wires.
 */
uint32_t
gj_sd_dev_mmcblk_prefix_len_6081(void)
{
	(void)NULL;
	return b6081_prefix_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_dev_mmcblk_prefix_len_6081(void)
    __attribute__((alias("gj_sd_dev_mmcblk_prefix_len_6081")));
