/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9724: btrfs receive ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_btrfs_receive_ok_u_9724(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       btrfs receive stream readiness probe.
 *   uint32_t __gj_btrfs_receive_ok_u_9724  (alias)
 *   __libcgj_batch9724_marker = "libcgj-batch9724"
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

const char __libcgj_batch9724_marker[] = "libcgj-batch9724";

/* Soft btrfs-receive-ok lamp: always off (not a real receive probe). */
#define B9724_BTRFS_RECEIVE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9724_btrfs_receive_ok(void)
{
	return B9724_BTRFS_RECEIVE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btrfs_receive_ok_u_9724 - btrfs receive ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run btrfs receive
 * or call libc. No parent wires.
 */
uint32_t
gj_btrfs_receive_ok_u_9724(void)
{
	(void)NULL;
	return b9724_btrfs_receive_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_btrfs_receive_ok_u_9724(void)
    __attribute__((alias("gj_btrfs_receive_ok_u_9724")));
