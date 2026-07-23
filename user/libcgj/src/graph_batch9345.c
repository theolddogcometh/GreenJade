/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9345: soft udev devnode-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_devnode_ok_u_9345(void);
 *     - Returns 1 (udev devnode soft-id continuum ok). Pure-data
 *       product tag; does not open device nodes or call libudev.
 *   uint32_t __gj_udev_devnode_ok_u_9345  (alias)
 *   __libcgj_batch9345_marker = "libcgj-batch9345"
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

const char __libcgj_batch9345_marker[] = "libcgj-batch9345";

#define B9345_UDEV_DEVNODE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9345_devnode_ok(void)
{
	return B9345_UDEV_DEVNODE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_devnode_ok_u_9345 - soft udev devnode soft-id continuum ok.
 *
 * Always returns 1. Soft pure-data product tag; does not open device
 * nodes or call libudev. No parent wires.
 */
uint32_t
gj_udev_devnode_ok_u_9345(void)
{
	(void)NULL;
	return b9345_devnode_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_devnode_ok_u_9345(void)
    __attribute__((alias("gj_udev_devnode_ok_u_9345")));
