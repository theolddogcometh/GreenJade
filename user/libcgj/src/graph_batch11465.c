/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11465: uinput soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uinput_ok_u_11465(void);
 *     - Soft uinput presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       /dev/uinput probe.
 *   uint32_t __gj_uinput_ok_u_11465  (alias)
 *   __libcgj_batch11465_marker = "libcgj-batch11465"
 *
 * Exclusive continuum CREATE-ONLY (11461-11470: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_11470).
 * Unique gj_uinput_ok_u_11465 surface only; no multi-def. Soft stub —
 * does not open uinput or inject events. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11465_marker[] = "libcgj-batch11465";

/* Soft unavailable / not-detected uinput lamp. */
#define B11465_UINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11465_uinput_ok(void)
{
	return B11465_UINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uinput_ok_u_11465 - uinput soft-id lamp.
 *
 * Always returns 0. Soft pure-data uinput presence tag. Does not open
 * uinput or call libc. No parent wires.
 */
uint32_t
gj_uinput_ok_u_11465(void)
{
	(void)NULL;
	return b11465_uinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uinput_ok_u_11465(void)
    __attribute__((alias("gj_uinput_ok_u_11465")));
