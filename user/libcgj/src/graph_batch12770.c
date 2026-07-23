/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12770: udev/libinput soft-id batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12770(void);
 *     - Returns the compile-time graph batch number for this TU (12770).
 *   uint32_t __gj_batch_id_12770  (alias)
 *   __libcgj_batch12770_marker = "libcgj-batch12770"
 *
 * Exclusive continuum CREATE-ONLY (12761-12770: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12770).
 * Unique gj_batch_id_12770 surface only; does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Distinct from gj_batch_id_12570 (batch12570), gj_batch_id_12370
 * (batch12370), gj_batch_id_12170 (batch12170), and peer lamps in this
 * wave. Soft pure-data catalog only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12770_marker[] = "libcgj-batch12770";

#define B12770_BATCH_ID  12770u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12770_id(void)
{
	return B12770_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12770 - report this TU's graph batch number.
 *
 * Always returns 12770. Soft pure-data batch identity for the
 * 12761-12770 udev/libinput soft-id wave. No parent wires.
 */
uint32_t
gj_batch_id_12770(void)
{
	(void)NULL;
	return b12770_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12770(void)
    __attribute__((alias("gj_batch_id_12770")));
