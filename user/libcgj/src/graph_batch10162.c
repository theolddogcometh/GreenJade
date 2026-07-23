/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10162: libinput soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_libinput_ok_u_10162(void);
 *     - Soft libinput presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       libinput context probe.
 *   uint32_t __gj_libinput_ok_u_10162  (alias)
 *   __libcgj_batch10162_marker = "libcgj-batch10162"
 *
 * Exclusive continuum CREATE-ONLY (10161-10170: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_10170).
 * Unique gj_libinput_ok_u_10162 surface only; no multi-def. Soft stub —
 * does not open libinput or enumerate seats. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10162_marker[] = "libcgj-batch10162";

/* Soft unavailable / not-detected libinput lamp. */
#define B10162_LIBINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10162_libinput_ok(void)
{
	return B10162_LIBINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libinput_ok_u_10162 - libinput soft-id lamp.
 *
 * Always returns 0. Soft pure-data libinput presence tag. Does not
 * probe libinput or call libc. No parent wires.
 */
uint32_t
gj_libinput_ok_u_10162(void)
{
	(void)NULL;
	return b10162_libinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libinput_ok_u_10162(void)
    __attribute__((alias("gj_libinput_ok_u_10162")));
