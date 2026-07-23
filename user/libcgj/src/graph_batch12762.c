/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12762: libinput soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libinput_ok_u_12762(void);
 *     - Soft libinput presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       libinput context probe.
 *   uint32_t __gj_libinput_ok_u_12762  (alias)
 *   __libcgj_batch12762_marker = "libcgj-batch12762"
 *
 * Exclusive continuum CREATE-ONLY (12761-12770: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12770).
 * Unique gj_libinput_ok_u_12762 surface only; no multi-def. Distinct
 * from gj_libinput_ok_u_12562 (batch12562), gj_libinput_ok_u_12362
 * (batch12362), gj_libinput_ok_u_12162 (batch12162), and peer lamps in
 * this wave. Soft stub — does not open libinput or enumerate seats.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12762_marker[] = "libcgj-batch12762";

/* Soft unavailable / not-detected libinput lamp. */
#define B12762_LIBINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12762_libinput_ok(void)
{
	return B12762_LIBINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libinput_ok_u_12762 - libinput soft-id lamp.
 *
 * Always returns 0. Soft pure-data libinput presence tag. Does not
 * open a libinput context or call libc. No parent wires.
 */
uint32_t
gj_libinput_ok_u_12762(void)
{
	(void)NULL;
	return b12762_libinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libinput_ok_u_12762(void)
    __attribute__((alias("gj_libinput_ok_u_12762")));
