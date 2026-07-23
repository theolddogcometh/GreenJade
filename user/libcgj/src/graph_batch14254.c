/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14254: soft udev subsystem-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_subsystem_ok_u_14254(void);
 *     - Returns 1 (udev subsystem soft-id continuum ok). Pure-data
 *       product tag; does not query subsystems or call libudev.
 *   uint32_t __gj_udev_subsystem_ok_u_14254  (alias)
 *   __libcgj_batch14254_marker = "libcgj-batch14254"
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

const char __libcgj_batch14254_marker[] = "libcgj-batch14254";

#define B14254_UDEV_SUBSYSTEM_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14254_subsystem_ok(void)
{
	return B14254_UDEV_SUBSYSTEM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_subsystem_ok_u_14254 - soft udev subsystem soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not query
 * subsystems or call libudev. No parent wires.
 */
uint32_t
gj_udev_subsystem_ok_u_14254(void)
{
	(void)NULL;
	return b14254_subsystem_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_subsystem_ok_u_14254(void)
    __attribute__((alias("gj_udev_subsystem_ok_u_14254")));
