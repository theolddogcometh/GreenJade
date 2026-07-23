/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11468: hwdb soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hwdb_ok_u_11468(void);
 *     - Soft hwdb presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       systemd/udev hwdb probe.
 *   uint32_t __gj_hwdb_ok_u_11468  (alias)
 *   __libcgj_batch11468_marker = "libcgj-batch11468"
 *
 * Exclusive continuum CREATE-ONLY (11461-11470: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_11470).
 * Unique gj_hwdb_ok_u_11468 surface only; no multi-def. Soft stub —
 * does not open hwdb.bin or query properties. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11468_marker[] = "libcgj-batch11468";

/* Soft unavailable / not-detected hwdb lamp. */
#define B11468_HWDB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11468_hwdb_ok(void)
{
	return B11468_HWDB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hwdb_ok_u_11468 - hwdb soft-id lamp.
 *
 * Always returns 0. Soft pure-data hwdb presence tag. Does not open
 * hwdb.bin or call libc. No parent wires.
 */
uint32_t
gj_hwdb_ok_u_11468(void)
{
	(void)NULL;
	return b11468_hwdb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hwdb_ok_u_11468(void)
    __attribute__((alias("gj_hwdb_ok_u_11468")));
