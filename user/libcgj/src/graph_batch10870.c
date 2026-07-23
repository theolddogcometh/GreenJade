/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10870: input soft continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10870(void);
 *     - Returns the compile-time graph batch number for this TU (10870).
 *   uint32_t __gj_batch_id_10870  (alias)
 *   __libcgj_batch10870_marker = "libcgj-batch10870"
 *
 * Exclusive continuum CREATE-ONLY (10861-10870: input soft —
 * udev_ok_u_10861, libinput_ok_u_10862, evdev_ok_u_10863,
 * hidraw_ok_u_10864, uinput_ok_u_10865, hotplug_ok_u_10866,
 * device_db_ok_u_10867, hwdb_ok_u_10868, input_soft_ready_u_10869,
 * batch_id_10870). Unique surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10870_marker[] = "libcgj-batch10870";

#define B10870_BATCH_ID  10870u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10870_id(void)
{
	return B10870_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10870 - report this TU's graph batch number.
 *
 * Always returns 10870. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker. Closes
 * the 10861-10870 input soft continuum.
 */
uint32_t
gj_batch_id_10870(void)
{
	(void)NULL;
	return b10870_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10870(void)
    __attribute__((alias("gj_batch_id_10870")));
