/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10861: input soft udev-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_ok_u_10861(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       udev/libudev presence probe for the input soft continuum.
 *   uint32_t __gj_udev_ok_u_10861  (alias)
 *   __libcgj_batch10861_marker = "libcgj-batch10861"
 *
 * Exclusive continuum CREATE-ONLY (10861-10870: input soft —
 * udev_ok_u_10861, libinput_ok_u_10862, evdev_ok_u_10863,
 * hidraw_ok_u_10864, uinput_ok_u_10865, hotplug_ok_u_10866,
 * device_db_ok_u_10867, hwdb_ok_u_10868, input_soft_ready_u_10869,
 * batch_id_10870). Unique surface only; no multi-def. Distinct from
 * prior input/udev soft lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10861_marker[] = "libcgj-batch10861";

/* Soft udev-ok lamp: always off (not a real udev probe). */
#define B10861_UDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10861_udev_ok(void)
{
	return B10861_UDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_ok_u_10861 - input soft udev-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open udev, scan
 * sysfs, or call libc. No parent wires.
 */
uint32_t
gj_udev_ok_u_10861(void)
{
	(void)NULL;
	return b10861_udev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_ok_u_10861(void)
    __attribute__((alias("gj_udev_ok_u_10861")));
