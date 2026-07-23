/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9716: soft ZFS recv-ok unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zfs_recv_ok_u_9716(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       zfs receive stream status probe for the zfs continuum.
 *   uint32_t __gj_zfs_recv_ok_u_9716  (alias)
 *   __libcgj_batch9716_marker = "libcgj-batch9716"
 *
 * Exclusive continuum CREATE-ONLY (9711-9720: zfs soft id stubs —
 * zfs_pool_ok_u_9711, zfs_dataset_ok_u_9712, zfs_snapshot_ok_u_9713,
 * zfs_clone_ok_u_9714, zfs_send_ok_u_9715, zfs_recv_ok_u_9716,
 * zfs_scrub_ok_u_9717, zfs_mount_ok_u_9718, zfs_ready_u_9719,
 * batch_id_9720). Unique surface only; no multi-def. Distinct from
 * zfs_send_ok_u_9715. No parent wires. No __int128. No ZFS
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9716_marker[] = "libcgj-batch9716";

/* Soft recv-ok lamp: always off (not a real receive-stream probe). */
#define B9716_ZFS_RECV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9716_recv_ok(void)
{
	return B9716_ZFS_RECV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zfs_recv_ok_u_9716 - ZFS recv soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not receive streams,
 * open pipes, or call ZFS. No parent wires.
 */
uint32_t
gj_zfs_recv_ok_u_9716(void)
{
	(void)NULL;
	return b9716_recv_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zfs_recv_ok_u_9716(void)
    __attribute__((alias("gj_zfs_recv_ok_u_9716")));
