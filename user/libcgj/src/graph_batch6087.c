/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6087: microSD /dev/disk/by-id path prefix length.
 *
 * Surface (unique symbols):
 *   uint32_t gj_microsd_by_id_prefix_len_6087(void);
 *     - Returns 16 (byte length of soft "/dev/disk/by-id/" prefix).
 *   uint32_t __gj_microsd_by_id_prefix_len_6087  (alias)
 *   __libcgj_batch6087_marker = "libcgj-batch6087"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_microsd_by_id_prefix_len_6087 surface only; no
 * multi-def. Distinct from mmcblk and sysfs prefixes. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6087_marker[] = "libcgj-batch6087";

/* Soft byte length of "/dev/disk/by-id/" (includes trailing slash). */
#define B6087_MICROSD_BY_ID_PREFIX_LEN  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6087_prefix_len(void)
{
	return B6087_MICROSD_BY_ID_PREFIX_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_microsd_by_id_prefix_len_6087 - /dev/disk/by-id/ path prefix length.
 *
 * Always returns 16. Soft compile-time stable-id path stub for microSD
 * / removable media; does not read udev links. No parent wires.
 */
uint32_t
gj_microsd_by_id_prefix_len_6087(void)
{
	(void)NULL;
	return b6087_prefix_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_microsd_by_id_prefix_len_6087(void)
    __attribute__((alias("gj_microsd_by_id_prefix_len_6087")));
