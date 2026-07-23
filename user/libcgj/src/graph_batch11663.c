/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11663: input soft evdev-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_11663(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       /dev/input/event* presence probe for the input soft continuum.
 *   uint32_t __gj_evdev_ok_u_11663  (alias)
 *   __libcgj_batch11663_marker = "libcgj-batch11663"
 *
 * Exclusive continuum CREATE-ONLY (11661-11670: input soft —
 * udev_ok_u_11661, libinput_ok_u_11662, evdev_ok_u_11663,
 * hidraw_ok_u_11664, uinput_ok_u_11665, hotplug_ok_u_11666,
 * device_db_ok_u_11667, hwdb_ok_u_11668, input_soft_ready_u_11669,
 * batch_id_11670). Unique surface only; no multi-def. Distinct from
 * gj_evdev_ok_u_11463 / gj_evdev_ok_u_11263 prior waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11663_marker[] = "libcgj-batch11663";

/* Soft evdev-ok lamp: always off (not a real /dev/input probe). */
#define B11663_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11663_evdev_ok(void)
{
	return B11663_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_11663 - input soft evdev-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open /dev/input,
 * scan event nodes, or call libc. No parent wires.
 */
uint32_t
gj_evdev_ok_u_11663(void)
{
	(void)NULL;
	return b11663_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_11663(void)
    __attribute__((alias("gj_evdev_ok_u_11663")));
