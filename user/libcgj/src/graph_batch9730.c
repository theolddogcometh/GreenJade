/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9730: btrfs soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9730(void);
 *     - Returns the compile-time graph batch number for this TU (9730).
 *   uint32_t __gj_batch_id_9730  (alias)
 *   __libcgj_batch9730_marker = "libcgj-batch9730"
 *
 * Exclusive continuum CREATE-ONLY (9721-9730: btrfs soft id
 * stubs — btrfs_subvol_ok_u_9721, btrfs_snapshot_ok_u_9722,
 * btrfs_send_ok_u_9723, btrfs_receive_ok_u_9724,
 * btrfs_balance_ok_u_9725, btrfs_scrub_ok_u_9726,
 * btrfs_device_ok_u_9727, btrfs_quota_ok_u_9728, btrfs_ready_u_9729,
 * batch_id_9730). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9730_marker[] = "libcgj-batch9730";

#define B9730_BATCH_ID  9730u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9730_id(void)
{
	return B9730_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9730 - report this TU's graph batch number.
 *
 * Always returns 9730.
 */
uint32_t
gj_batch_id_9730(void)
{
	(void)NULL;
	return b9730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9730(void)
    __attribute__((alias("gj_batch_id_9730")));
