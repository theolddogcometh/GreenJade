/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12167: device_db soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_device_db_ok_u_12167(void);
 *     - Soft device-db presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       udev device database probe.
 *   uint32_t __gj_device_db_ok_u_12167  (alias)
 *   __libcgj_batch12167_marker = "libcgj-batch12167"
 *
 * Exclusive continuum CREATE-ONLY (12161-12170: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12170).
 * Unique gj_device_db_ok_u_12167 surface only; no multi-def. Soft stub —
 * does not open device.db or enumerate properties. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12167_marker[] = "libcgj-batch12167";

/* Soft unavailable / not-detected device_db lamp. */
#define B12167_DEVICE_DB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12167_device_db_ok(void)
{
	return B12167_DEVICE_DB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_device_db_ok_u_12167 - device_db soft-id lamp.
 *
 * Always returns 0. Soft pure-data device database presence tag. Does
 * not probe udev db or call libc. No parent wires.
 */
uint32_t
gj_device_db_ok_u_12167(void)
{
	(void)NULL;
	return b12167_device_db_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_device_db_ok_u_12167(void)
    __attribute__((alias("gj_device_db_ok_u_12167")));
