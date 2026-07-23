/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12368: hwdb soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hwdb_ok_u_12368(void);
 *     - Soft hwdb presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       udev hardware database probe.
 *   uint32_t __gj_hwdb_ok_u_12368  (alias)
 *   __libcgj_batch12368_marker = "libcgj-batch12368"
 *
 * Exclusive continuum CREATE-ONLY (12361-12370: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12370).
 * Unique gj_hwdb_ok_u_12368 surface only; no multi-def. Soft stub —
 * does not open hwdb.bin or query properties. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12368_marker[] = "libcgj-batch12368";

/* Soft unavailable / not-detected hwdb lamp. */
#define B12368_HWDB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12368_hwdb_ok(void)
{
	return B12368_HWDB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hwdb_ok_u_12368 - hwdb soft-id lamp.
 *
 * Always returns 0. Soft pure-data hardware database presence tag.
 * Does not probe hwdb or call libc. No parent wires.
 */
uint32_t
gj_hwdb_ok_u_12368(void)
{
	(void)NULL;
	return b12368_hwdb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hwdb_ok_u_12368(void)
    __attribute__((alias("gj_hwdb_ok_u_12368")));
