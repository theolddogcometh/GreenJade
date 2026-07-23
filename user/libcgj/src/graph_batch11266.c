/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11266: input soft hotplug-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hotplug_ok_u_11266(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       input hotplug/monitor presence probe for the input soft
 *       continuum.
 *   uint32_t __gj_hotplug_ok_u_11266  (alias)
 *   __libcgj_batch11266_marker = "libcgj-batch11266"
 *
 * Exclusive continuum CREATE-ONLY (11261-11270: input soft —
 * udev_ok_u_11261, libinput_ok_u_11262, evdev_ok_u_11263,
 * hidraw_ok_u_11264, uinput_ok_u_11265, hotplug_ok_u_11266,
 * device_db_ok_u_11267, hwdb_ok_u_11268, input_soft_ready_u_11269,
 * batch_id_11270). Unique surface only; no multi-def. Distinct from
 * gj_hotplug_ok_u_11066 / gj_hotplug_ok_u_10866 prior waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11266_marker[] = "libcgj-batch11266";

/* Soft hotplug-ok lamp: always off (not a real hotplug probe). */
#define B11266_HOTPLUG_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11266_hotplug_ok(void)
{
	return B11266_HOTPLUG_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hotplug_ok_u_11266 - input soft hotplug-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not watch netlink or call
 * libc. No parent wires.
 */
uint32_t
gj_hotplug_ok_u_11266(void)
{
	(void)NULL;
	return b11266_hotplug_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hotplug_ok_u_11266(void)
    __attribute__((alias("gj_hotplug_ok_u_11266")));
