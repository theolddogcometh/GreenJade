/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9717: soft ZFS scrub-ok unit (not a probe).
 *
 * Surface (unique symbols):
 *   uint32_t gj_zfs_scrub_ok_u_9717(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       pool scrub status probe for the zfs continuum.
 *   uint32_t __gj_zfs_scrub_ok_u_9717  (alias)
 *   __libcgj_batch9717_marker = "libcgj-batch9717"
 *
 * Exclusive continuum CREATE-ONLY (9711-9720: zfs soft id stubs —
 * zfs_pool_ok_u_9711, zfs_dataset_ok_u_9712, zfs_snapshot_ok_u_9713,
 * zfs_clone_ok_u_9714, zfs_send_ok_u_9715, zfs_recv_ok_u_9716,
 * zfs_scrub_ok_u_9717, zfs_mount_ok_u_9718, zfs_ready_u_9719,
 * batch_id_9720). Unique surface only; no multi-def. Distinct from
 * send/recv ok soft lamps. No parent wires. No __int128. No ZFS
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9717_marker[] = "libcgj-batch9717";

/* Soft scrub-ok lamp: always off (not a real scrub probe). */
#define B9717_ZFS_SCRUB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9717_scrub_ok(void)
{
	return B9717_ZFS_SCRUB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zfs_scrub_ok_u_9717 - ZFS scrub soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not start scrubs,
 * query scrub state, or call ZFS. No parent wires.
 */
uint32_t
gj_zfs_scrub_ok_u_9717(void)
{
	(void)NULL;
	return b9717_scrub_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zfs_scrub_ok_u_9717(void)
    __attribute__((alias("gj_zfs_scrub_ok_u_9717")));
