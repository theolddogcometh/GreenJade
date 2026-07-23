/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10868: input soft hwdb-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hwdb_ok_u_10868(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       udev hwdb presence probe for the input soft continuum.
 *   uint32_t __gj_hwdb_ok_u_10868  (alias)
 *   __libcgj_batch10868_marker = "libcgj-batch10868"
 *
 * Exclusive continuum CREATE-ONLY (10861-10870: input soft —
 * udev_ok_u_10861, libinput_ok_u_10862, evdev_ok_u_10863,
 * hidraw_ok_u_10864, uinput_ok_u_10865, hotplug_ok_u_10866,
 * device_db_ok_u_10867, hwdb_ok_u_10868, input_soft_ready_u_10869,
 * batch_id_10870). Unique surface only; no multi-def. Distinct from
 * prior hwdb soft lamps. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10868_marker[] = "libcgj-batch10868";

/* Soft hwdb-ok lamp: always off (not a real hwdb.bin probe). */
#define B10868_HWDB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10868_hwdb_ok(void)
{
	return B10868_HWDB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hwdb_ok_u_10868 - input soft hwdb-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open hwdb.bin, query
 * properties, or call libc. No parent wires.
 */
uint32_t
gj_hwdb_ok_u_10868(void)
{
	(void)NULL;
	return b10868_hwdb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hwdb_ok_u_10868(void)
    __attribute__((alias("gj_hwdb_ok_u_10868")));
