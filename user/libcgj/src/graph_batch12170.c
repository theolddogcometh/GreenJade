/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12170: udev/libinput soft-id batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_12170(void);
 *     - Returns the compile-time graph batch number for this TU (12170).
 *   uint32_t __gj_batch_id_12170  (alias)
 *   __libcgj_batch12170_marker = "libcgj-batch12170"
 *
 * Exclusive continuum CREATE-ONLY (12161-12170: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12170).
 * Unique gj_batch_id_12170 surface only; does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def. Soft
 * pure-data catalog only. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12170_marker[] = "libcgj-batch12170";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12170_id(void)
{
	return 12170u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_12170 - report this TU's graph batch number.
 *
 * Always returns 12170. Soft pure-data batch identity for the
 * 12161-12170 udev/libinput soft-id wave. No parent wires.
 */
uint32_t
gj_batch_id_12170(void)
{
	(void)NULL;
	return b12170_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_12170(void)
    __attribute__((alias("gj_batch_id_12170")));
