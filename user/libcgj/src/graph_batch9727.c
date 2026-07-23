/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9727: btrfs device ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_btrfs_device_ok_u_9727(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       btrfs device/add/remove readiness probe.
 *   uint32_t __gj_btrfs_device_ok_u_9727  (alias)
 *   __libcgj_batch9727_marker = "libcgj-batch9727"
 *
 * Exclusive continuum CREATE-ONLY (9721-9730: btrfs soft id
 * stubs — btrfs_subvol_ok_u_9721, btrfs_snapshot_ok_u_9722,
 * btrfs_send_ok_u_9723, btrfs_receive_ok_u_9724,
 * btrfs_balance_ok_u_9725, btrfs_scrub_ok_u_9726,
 * btrfs_device_ok_u_9727, btrfs_quota_ok_u_9728, btrfs_ready_u_9729,
 * batch_id_9730). Unique surface only; no multi-def. No parent wires.
 * No __int128. No btrfs implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9727_marker[] = "libcgj-batch9727";

/* Soft btrfs-device-ok lamp: always off (not a real device probe). */
#define B9727_BTRFS_DEVICE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9727_btrfs_device_ok(void)
{
	return B9727_BTRFS_DEVICE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btrfs_device_ok_u_9727 - btrfs device ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect devices
 * or call libc. No parent wires.
 */
uint32_t
gj_btrfs_device_ok_u_9727(void)
{
	(void)NULL;
	return b9727_btrfs_device_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_btrfs_device_ok_u_9727(void)
    __attribute__((alias("gj_btrfs_device_ok_u_9727")));
