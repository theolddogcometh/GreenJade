/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10667: input soft device-db-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_device_db_ok_u_10667(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       input device database presence probe for the input soft
 *       continuum.
 *   uint32_t __gj_device_db_ok_u_10667  (alias)
 *   __libcgj_batch10667_marker = "libcgj-batch10667"
 *
 * Exclusive continuum CREATE-ONLY (10661-10670: input soft —
 * udev_ok_u_10661, libinput_ok_u_10662, evdev_ok_u_10663,
 * hidraw_ok_u_10664, uinput_ok_u_10665, hotplug_ok_u_10666,
 * device_db_ok_u_10667, hwdb_ok_u_10668, input_soft_ready_u_10669,
 * batch_id_10670). Unique surface only; no multi-def. Distinct from
 * prior device-db soft lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10667_marker[] = "libcgj-batch10667";

/* Soft device-db-ok lamp: always off (not a real device-db probe). */
#define B10667_DEVICE_DB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10667_device_db_ok(void)
{
	return B10667_DEVICE_DB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_device_db_ok_u_10667 - input soft device-db-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open device
 * databases, parse ID files, or call libc. No parent wires.
 */
uint32_t
gj_device_db_ok_u_10667(void)
{
	(void)NULL;
	return b10667_device_db_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_device_db_ok_u_10667(void)
    __attribute__((alias("gj_device_db_ok_u_10667")));
