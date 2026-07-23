/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11870: input soft continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11870(void);
 *     - Returns the compile-time graph batch number for this TU (11870).
 *   uint32_t __gj_batch_id_11870  (alias)
 *   __libcgj_batch11870_marker = "libcgj-batch11870"
 *
 * Exclusive continuum CREATE-ONLY (11861-11870: input soft —
 * udev_ok_u_11861, libinput_ok_u_11862, evdev_ok_u_11863,
 * hidraw_ok_u_11864, uinput_ok_u_11865, hotplug_ok_u_11866,
 * device_db_ok_u_11867, hwdb_ok_u_11868, input_soft_ready_u_11869,
 * batch_id_11870). Unique surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. Distinct from gj_batch_id_11670 / gj_batch_id_11470. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11870_marker[] = "libcgj-batch11870";

#define B11870_BATCH_ID  11870u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11870_id(void)
{
	return B11870_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11870 - report this TU's graph batch number.
 *
 * Always returns 11870. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker. Closes
 * the 11861-11870 input soft continuum.
 */
uint32_t
gj_batch_id_11870(void)
{
	(void)NULL;
	return b11870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11870(void)
    __attribute__((alias("gj_batch_id_11870")));
