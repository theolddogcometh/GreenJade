/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11469: input soft-ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_input_soft_ready_u_11469(void);
 *     - Soft udev/libinput graph ready lamp (always 1 — soft surface
 *       ready for the 11461-11470 udev/libinput soft-id wave).
 *       Compile-time pure-data product tag; not a live device enum.
 *   uint32_t __gj_input_soft_ready_u_11469  (alias)
 *   __libcgj_batch11469_marker = "libcgj-batch11469"
 *
 * Exclusive continuum CREATE-ONLY (11461-11470: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_11470).
 * Unique gj_input_soft_ready_u_11469 surface only; no multi-def. Soft
 * pure-data ready tag. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11469_marker[] = "libcgj-batch11469";

/* Input soft surface ready lamp. */
#define B11469_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11469_soft_ready(void)
{
	return B11469_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_input_soft_ready_u_11469 - input soft-ready lamp.
 *
 * Always returns 1. Soft pure-data product tag for the 11461-11470
 * udev/libinput soft-id wave. Does not enumerate devices or call
 * libc. No parent wires.
 */
uint32_t
gj_input_soft_ready_u_11469(void)
{
	(void)NULL;
	return b11469_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_input_soft_ready_u_11469(void)
    __attribute__((alias("gj_input_soft_ready_u_11469")));
