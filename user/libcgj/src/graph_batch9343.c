/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9343: soft udev action-change id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_action_change_id_9343(void);
 *     - Returns 3 (udev "change" action soft catalog id). Pure-data
 *       action id stub; does not watch devices or call libudev.
 *   uint32_t __gj_udev_action_change_id_9343  (alias)
 *   __libcgj_batch9343_marker = "libcgj-batch9343"
 *
 * Exclusive continuum CREATE-ONLY (9341-9350: udev soft id stubs —
 * udev_action_add_id_9341, udev_action_remove_id_9342,
 * udev_action_change_id_9343, udev_subsystem_ok_u_9344,
 * udev_devnode_ok_u_9345, udev_syspath_ok_u_9346,
 * udev_monitor_ok_u_9347, udev_enumerate_ok_u_9348,
 * udev_ready_u_9349, batch_id_9350).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No udev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9343_marker[] = "libcgj-batch9343";

/* Soft udev "change" action catalog id. */
#define B9343_UDEV_ACTION_CHANGE  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9343_action_change_id(void)
{
	return B9343_UDEV_ACTION_CHANGE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_action_change_id_9343 - soft udev "change" action catalog id.
 *
 * Always returns 3. Soft continuum constant; does not watch devices or
 * call libudev. No parent wires.
 */
uint32_t
gj_udev_action_change_id_9343(void)
{
	(void)NULL;
	return b9343_action_change_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_action_change_id_9343(void)
    __attribute__((alias("gj_udev_action_change_id_9343")));
