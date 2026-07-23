/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11270: input soft continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11270(void);
 *     - Returns the compile-time graph batch number for this TU (11270).
 *   uint32_t __gj_batch_id_11270  (alias)
 *   __libcgj_batch11270_marker = "libcgj-batch11270"
 *
 * Exclusive continuum CREATE-ONLY (11261-11270: input soft —
 * udev_ok_u_11261, libinput_ok_u_11262, evdev_ok_u_11263,
 * hidraw_ok_u_11264, uinput_ok_u_11265, hotplug_ok_u_11266,
 * device_db_ok_u_11267, hwdb_ok_u_11268, input_soft_ready_u_11269,
 * batch_id_11270). Unique surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. Distinct from gj_batch_id_11070 / gj_batch_id_10870. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11270_marker[] = "libcgj-batch11270";

#define B11270_BATCH_ID  11270u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11270_id(void)
{
	return B11270_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11270 - report this TU's graph batch number.
 *
 * Always returns 11270. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker. Closes
 * the 11261-11270 input soft continuum.
 */
uint32_t
gj_batch_id_11270(void)
{
	(void)NULL;
	return b11270_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11270(void)
    __attribute__((alias("gj_batch_id_11270")));
