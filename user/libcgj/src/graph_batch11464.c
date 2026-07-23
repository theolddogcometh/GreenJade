/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11464: hidraw soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_11464(void);
 *     - Soft hidraw presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       /dev/hidraw* probe.
 *   uint32_t __gj_hidraw_ok_u_11464  (alias)
 *   __libcgj_batch11464_marker = "libcgj-batch11464"
 *
 * Exclusive continuum CREATE-ONLY (11461-11470: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_11470).
 * Unique gj_hidraw_ok_u_11464 surface only; no multi-def. Soft stub —
 * does not open hidraw nodes or enumerate HID reports. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11464_marker[] = "libcgj-batch11464";

/* Soft unavailable / not-detected hidraw lamp. */
#define B11464_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11464_hidraw_ok(void)
{
	return B11464_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_11464 - hidraw soft-id lamp.
 *
 * Always returns 0. Soft pure-data hidraw presence tag. Does not open
 * hidraw nodes or call libc. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_11464(void)
{
	(void)NULL;
	return b11464_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_11464(void)
    __attribute__((alias("gj_hidraw_ok_u_11464")));
