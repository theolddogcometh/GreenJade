/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10466: input soft hotplug-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hotplug_ok_u_10466(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       input hotplug monitor probe for the input soft continuum.
 *   uint32_t __gj_hotplug_ok_u_10466  (alias)
 *   __libcgj_batch10466_marker = "libcgj-batch10466"
 *
 * Exclusive continuum CREATE-ONLY (10461-10470: input soft all→0 —
 * udev_ok_u_10461, libinput_ok_u_10462, evdev_ok_u_10463,
 * hidraw_ok_u_10464, uinput_ok_u_10465, hotplug_ok_u_10466,
 * device_db_ok_u_10467, hwdb_ok_u_10468, input_soft_ready_u_10469,
 * batch_id_10470). Unique surface only; no multi-def. Distinct from
 * prior hotplug soft lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10466_marker[] = "libcgj-batch10466";

/* Soft hotplug-ok lamp: always off (not a real hotplug monitor). */
#define B10466_HOTPLUG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10466_hotplug_ok(void)
{
	return B10466_HOTPLUG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hotplug_ok_u_10466 - input soft hotplug-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not watch udev netlink,
 * poll inotify on /dev/input, or call libc. No parent wires.
 */
uint32_t
gj_hotplug_ok_u_10466(void)
{
	(void)NULL;
	return b10466_hotplug_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hotplug_ok_u_10466(void)
    __attribute__((alias("gj_hotplug_ok_u_10466")));
