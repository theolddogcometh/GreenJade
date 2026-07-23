/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9713: soft ZFS snapshot-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zfs_snapshot_ok_u_9713(void);
 *     - Returns 1 (zfs snapshot soft-id continuum ok). Pure-data product
 *       tag; does not create snapshots or call ZFS.
 *   uint32_t __gj_zfs_snapshot_ok_u_9713  (alias)
 *   __libcgj_batch9713_marker = "libcgj-batch9713"
 *
 * Exclusive continuum CREATE-ONLY (9711-9720: zfs soft id stubs —
 * zfs_pool_ok_u_9711, zfs_dataset_ok_u_9712, zfs_snapshot_ok_u_9713,
 * zfs_clone_ok_u_9714, zfs_send_ok_u_9715, zfs_recv_ok_u_9716,
 * zfs_scrub_ok_u_9717, zfs_mount_ok_u_9718, zfs_ready_u_9719,
 * batch_id_9720). Unique surface only; no multi-def. Distinct from
 * pool/dataset ok flags. No parent wires. No __int128. No ZFS
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9713_marker[] = "libcgj-batch9713";

#define B9713_ZFS_SNAPSHOT_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9713_snapshot_ok(void)
{
	return B9713_ZFS_SNAPSHOT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zfs_snapshot_ok_u_9713 - ZFS snapshot soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not create
 * snapshots or call ZFS. No parent wires.
 */
uint32_t
gj_zfs_snapshot_ok_u_9713(void)
{
	(void)NULL;
	return b9713_snapshot_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zfs_snapshot_ok_u_9713(void)
    __attribute__((alias("gj_zfs_snapshot_ok_u_9713")));
