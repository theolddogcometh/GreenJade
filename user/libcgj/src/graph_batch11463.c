/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11463: evdev soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_11463(void);
 *     - Soft evdev presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       /dev/input/event* probe.
 *   uint32_t __gj_evdev_ok_u_11463  (alias)
 *   __libcgj_batch11463_marker = "libcgj-batch11463"
 *
 * Exclusive continuum CREATE-ONLY (11461-11470: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_11470).
 * Unique gj_evdev_ok_u_11463 surface only; no multi-def. Soft stub —
 * does not open event nodes or ioctl EVIOC*. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11463_marker[] = "libcgj-batch11463";

/* Soft unavailable / not-detected evdev lamp. */
#define B11463_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11463_evdev_ok(void)
{
	return B11463_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_11463 - evdev soft-id lamp.
 *
 * Always returns 0. Soft pure-data evdev presence tag. Does not open
 * event nodes or call libc. No parent wires.
 */
uint32_t
gj_evdev_ok_u_11463(void)
{
	(void)NULL;
	return b11463_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_11463(void)
    __attribute__((alias("gj_evdev_ok_u_11463")));
