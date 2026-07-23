/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11267: input soft device-db-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_device_db_ok_u_11267(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       device database presence probe for the input soft continuum.
 *   uint32_t __gj_device_db_ok_u_11267  (alias)
 *   __libcgj_batch11267_marker = "libcgj-batch11267"
 *
 * Exclusive continuum CREATE-ONLY (11261-11270: input soft —
 * udev_ok_u_11261, libinput_ok_u_11262, evdev_ok_u_11263,
 * hidraw_ok_u_11264, uinput_ok_u_11265, hotplug_ok_u_11266,
 * device_db_ok_u_11267, hwdb_ok_u_11268, input_soft_ready_u_11269,
 * batch_id_11270). Unique surface only; no multi-def. Distinct from
 * gj_device_db_ok_u_11067 / gj_device_db_ok_u_10867 prior waves. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11267_marker[] = "libcgj-batch11267";

/* Soft device-db-ok lamp: always off (not a real device-db probe). */
#define B11267_DEVICE_DB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11267_device_db_ok(void)
{
	return B11267_DEVICE_DB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_device_db_ok_u_11267 - input soft device-db-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open a device
 * database or call libc. No parent wires.
 */
uint32_t
gj_device_db_ok_u_11267(void)
{
	(void)NULL;
	return b11267_device_db_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_device_db_ok_u_11267(void)
    __attribute__((alias("gj_device_db_ok_u_11267")));
