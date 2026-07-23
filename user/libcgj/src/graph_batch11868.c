/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11868: input soft hwdb-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hwdb_ok_u_11868(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       udev hwdb presence probe for the input soft continuum.
 *   uint32_t __gj_hwdb_ok_u_11868  (alias)
 *   __libcgj_batch11868_marker = "libcgj-batch11868"
 *
 * Exclusive continuum CREATE-ONLY (11861-11870: input soft —
 * udev_ok_u_11861, libinput_ok_u_11862, evdev_ok_u_11863,
 * hidraw_ok_u_11864, uinput_ok_u_11865, hotplug_ok_u_11866,
 * device_db_ok_u_11867, hwdb_ok_u_11868, input_soft_ready_u_11869,
 * batch_id_11870). Unique surface only; no multi-def. Distinct from
 * gj_hwdb_ok_u_11668 / gj_hwdb_ok_u_11468 prior waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11868_marker[] = "libcgj-batch11868";

/* Soft hwdb-ok lamp: always off (not a real hwdb probe). */
#define B11868_HWDB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11868_hwdb_ok(void)
{
	return B11868_HWDB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hwdb_ok_u_11868 - input soft hwdb-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open udev hwdb,
 * query properties, or call libc. No parent wires.
 */
uint32_t
gj_hwdb_ok_u_11868(void)
{
	(void)NULL;
	return b11868_hwdb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hwdb_ok_u_11868(void)
    __attribute__((alias("gj_hwdb_ok_u_11868")));
