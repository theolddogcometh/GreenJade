/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9720: zfs soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9720(void);
 *     - Returns the compile-time graph batch number for this TU (9720).
 *   uint32_t __gj_batch_id_9720  (alias)
 *   __libcgj_batch9720_marker = "libcgj-batch9720"
 *
 * Exclusive continuum CREATE-ONLY (9711-9720: zfs soft id stubs —
 * zfs_pool_ok_u_9711, zfs_dataset_ok_u_9712, zfs_snapshot_ok_u_9713,
 * zfs_clone_ok_u_9714, zfs_send_ok_u_9715, zfs_recv_ok_u_9716,
 * zfs_scrub_ok_u_9717, zfs_mount_ok_u_9718, zfs_ready_u_9719,
 * batch_id_9720). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128. No ZFS implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9720_marker[] = "libcgj-batch9720";

#define B9720_BATCH_ID  9720u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9720_id(void)
{
	return B9720_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9720 - report this TU's graph batch number.
 *
 * Always returns 9720. Link-time presence tags the exclusive
 * zfs soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9720(void)
{
	(void)NULL;
	return b9720_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9720(void)
    __attribute__((alias("gj_batch_id_9720")));
