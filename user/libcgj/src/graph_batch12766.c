/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12766: hotplug soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hotplug_ok_u_12766(void);
 *     - Soft hotplug presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       udev/netlink hotplug monitor.
 *   uint32_t __gj_hotplug_ok_u_12766  (alias)
 *   __libcgj_batch12766_marker = "libcgj-batch12766"
 *
 * Exclusive continuum CREATE-ONLY (12761-12770: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12770).
 * Unique gj_hotplug_ok_u_12766 surface only; no multi-def. Distinct
 * from gj_hotplug_ok_u_12566 (batch12566), gj_hotplug_ok_u_12366
 * (batch12366), gj_hotplug_ok_u_12166 (batch12166), and peer lamps in
 * this wave. Soft stub — does not subscribe to netlink or watch device
 * add/remove. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12766_marker[] = "libcgj-batch12766";

/* Soft unavailable / not-detected hotplug lamp. */
#define B12766_HOTPLUG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12766_hotplug_ok(void)
{
	return B12766_HOTPLUG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hotplug_ok_u_12766 - hotplug soft-id lamp.
 *
 * Always returns 0. Soft pure-data hotplug presence tag. Does not
 * subscribe to netlink or call libc. No parent wires.
 */
uint32_t
gj_hotplug_ok_u_12766(void)
{
	(void)NULL;
	return b12766_hotplug_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hotplug_ok_u_12766(void)
    __attribute__((alias("gj_hotplug_ok_u_12766")));
