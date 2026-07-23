/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12161: udev soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_ok_u_12161(void);
 *     - Soft udev presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       udev/netlink device probe.
 *   uint32_t __gj_udev_ok_u_12161  (alias)
 *   __libcgj_batch12161_marker = "libcgj-batch12161"
 *
 * Exclusive continuum CREATE-ONLY (12161-12170: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12170).
 * Unique gj_udev_ok_u_12161 surface only; no multi-def. Soft stub —
 * does not open udev sockets or enumerate devices. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12161_marker[] = "libcgj-batch12161";

/* Soft unavailable / not-detected udev lamp. */
#define B12161_UDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12161_udev_ok(void)
{
	return B12161_UDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_ok_u_12161 - udev soft-id lamp.
 *
 * Always returns 0. Soft pure-data udev presence tag. Does not probe
 * udev/netlink or call libc. No parent wires.
 */
uint32_t
gj_udev_ok_u_12161(void)
{
	(void)NULL;
	return b12161_udev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_ok_u_12161(void)
    __attribute__((alias("gj_udev_ok_u_12161")));
