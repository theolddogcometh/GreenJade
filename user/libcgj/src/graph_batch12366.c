/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12366: hotplug soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hotplug_ok_u_12366(void);
 *     - Soft hotplug presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       udev/netlink hotplug monitor.
 *   uint32_t __gj_hotplug_ok_u_12366  (alias)
 *   __libcgj_batch12366_marker = "libcgj-batch12366"
 *
 * Exclusive continuum CREATE-ONLY (12361-12370: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12370).
 * Unique gj_hotplug_ok_u_12366 surface only; no multi-def. Soft stub —
 * does not subscribe to netlink or watch device add/remove. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12366_marker[] = "libcgj-batch12366";

/* Soft unavailable / not-detected hotplug lamp. */
#define B12366_HOTPLUG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12366_hotplug_ok(void)
{
	return B12366_HOTPLUG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hotplug_ok_u_12366 - hotplug soft-id lamp.
 *
 * Always returns 0. Soft pure-data hotplug presence tag. Does not
 * monitor udev events or call libc. No parent wires.
 */
uint32_t
gj_hotplug_ok_u_12366(void)
{
	(void)NULL;
	return b12366_hotplug_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hotplug_ok_u_12366(void)
    __attribute__((alias("gj_hotplug_ok_u_12366")));
