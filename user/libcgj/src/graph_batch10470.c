/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10470: input soft continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_10470(void);
 *     - Returns the compile-time graph batch number for this TU (10470).
 *   uint32_t __gj_batch_id_10470  (alias)
 *   __libcgj_batch10470_marker = "libcgj-batch10470"
 *
 * Exclusive continuum CREATE-ONLY (10461-10470: input soft all→0 —
 * udev_ok_u_10461, libinput_ok_u_10462, evdev_ok_u_10463,
 * hidraw_ok_u_10464, uinput_ok_u_10465, hotplug_ok_u_10466,
 * device_db_ok_u_10467, hwdb_ok_u_10468, input_soft_ready_u_10469,
 * batch_id_10470). Unique surface only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10470_marker[] = "libcgj-batch10470";

#define B10470_BATCH_ID  10470u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10470_id(void)
{
	return B10470_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_10470 - report this TU's graph batch number.
 *
 * Always returns 10470. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker. Closes
 * the 10461-10470 input soft continuum.
 */
uint32_t
gj_batch_id_10470(void)
{
	(void)NULL;
	return b10470_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_10470(void)
    __attribute__((alias("gj_batch_id_10470")));
