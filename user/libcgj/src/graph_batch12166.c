/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12166: hotplug soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hotplug_ok_u_12166(void);
 *     - Soft hotplug presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       netlink/uevent hotplug probe.
 *   uint32_t __gj_hotplug_ok_u_12166  (alias)
 *   __libcgj_batch12166_marker = "libcgj-batch12166"
 *
 * Exclusive continuum CREATE-ONLY (12161-12170: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12170).
 * Unique gj_hotplug_ok_u_12166 surface only; no multi-def. Soft stub —
 * does not open netlink or watch uevents. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12166_marker[] = "libcgj-batch12166";

/* Soft unavailable / not-detected hotplug lamp. */
#define B12166_HOTPLUG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12166_hotplug_ok(void)
{
	return B12166_HOTPLUG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hotplug_ok_u_12166 - hotplug soft-id lamp.
 *
 * Always returns 0. Soft pure-data hotplug presence tag. Does not probe
 * netlink/uevent or call libc. No parent wires.
 */
uint32_t
gj_hotplug_ok_u_12166(void)
{
	(void)NULL;
	return b12166_hotplug_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hotplug_ok_u_12166(void)
    __attribute__((alias("gj_hotplug_ok_u_12166")));
