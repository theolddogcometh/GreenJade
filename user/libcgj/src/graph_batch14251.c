/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14251: soft udev action-add id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_action_add_id_14251(void);
 *     - Returns 1 (udev "add" action soft catalog id). Pure-data
 *       action id stub; does not watch devices or call libudev.
 *   uint32_t __gj_udev_action_add_id_14251  (alias)
 *   __libcgj_batch14251_marker = "libcgj-batch14251"
 *
 * Exclusive continuum CREATE-ONLY (14251-14260: udev soft id stubs —
 * udev_action_add_id_14251, udev_action_remove_id_14252,
 * udev_action_change_id_14253, udev_subsystem_ok_u_14254,
 * udev_devnode_ok_u_14255, udev_syspath_ok_u_14256,
 * udev_monitor_ok_u_14257, udev_enumerate_ok_u_14258,
 * udev_ready_u_14259, batch_id_14260).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No udev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14251_marker[] = "libcgj-batch14251";

/* Soft udev "add" action catalog id. */
#define B14251_UDEV_ACTION_ADD  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14251_action_add_id(void)
{
	return B14251_UDEV_ACTION_ADD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_action_add_id_14251 - soft udev "add" action catalog id.
 *
 * Always returns 1. Soft continuum constant; does not watch devices or
 * call libudev. No parent wires.
 */
uint32_t
gj_udev_action_add_id_14251(void)
{
	(void)NULL;
	return b14251_action_add_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_action_add_id_14251(void)
    __attribute__((alias("gj_udev_action_add_id_14251")));
