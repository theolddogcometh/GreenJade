/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11862: input soft libinput-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_libinput_ok_u_11862(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       libinput presence probe for the input soft continuum.
 *   uint32_t __gj_libinput_ok_u_11862  (alias)
 *   __libcgj_batch11862_marker = "libcgj-batch11862"
 *
 * Exclusive continuum CREATE-ONLY (11861-11870: input soft —
 * udev_ok_u_11861, libinput_ok_u_11862, evdev_ok_u_11863,
 * hidraw_ok_u_11864, uinput_ok_u_11865, hotplug_ok_u_11866,
 * device_db_ok_u_11867, hwdb_ok_u_11868, input_soft_ready_u_11869,
 * batch_id_11870). Unique surface only; no multi-def. Distinct from
 * gj_libinput_ok_u_11662 / gj_libinput_ok_u_11462 prior waves. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11862_marker[] = "libcgj-batch11862";

/* Soft libinput-ok lamp: always off (not a real libinput probe). */
#define B11862_LIBINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11862_libinput_ok(void)
{
	return B11862_LIBINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_libinput_ok_u_11862 - input soft libinput-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open devices, call
 * libinput, or call libc. No parent wires.
 */
uint32_t
gj_libinput_ok_u_11862(void)
{
	(void)NULL;
	return b11862_libinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_libinput_ok_u_11862(void)
    __attribute__((alias("gj_libinput_ok_u_11862")));
