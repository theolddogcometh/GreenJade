/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10670: input soft continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10670(void);
 *     - Returns the compile-time graph batch number for this TU (10670).
 *   uint32_t __gj_batch_id_10670  (alias)
 *   __libcgj_batch10670_marker = "libcgj-batch10670"
 *
 * Exclusive continuum CREATE-ONLY (10661-10670: input soft —
 * udev_ok_u_10661, libinput_ok_u_10662, evdev_ok_u_10663,
 * hidraw_ok_u_10664, uinput_ok_u_10665, hotplug_ok_u_10666,
 * device_db_ok_u_10667, hwdb_ok_u_10668, input_soft_ready_u_10669,
 * batch_id_10670). Unique surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10670_marker[] = "libcgj-batch10670";

#define B10670_BATCH_ID  10670u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10670_id(void)
{
	return B10670_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10670 - report this TU's graph batch number.
 *
 * Always returns 10670. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker. Closes
 * the 10661-10670 input soft continuum.
 */
uint32_t
gj_batch_id_10670(void)
{
	(void)NULL;
	return b10670_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10670(void)
    __attribute__((alias("gj_batch_id_10670")));
