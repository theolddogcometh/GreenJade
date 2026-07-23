/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12768: hwdb soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hwdb_ok_u_12768(void);
 *     - Soft hwdb presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       udev hardware database probe.
 *   uint32_t __gj_hwdb_ok_u_12768  (alias)
 *   __libcgj_batch12768_marker = "libcgj-batch12768"
 *
 * Exclusive continuum CREATE-ONLY (12761-12770: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12770).
 * Unique gj_hwdb_ok_u_12768 surface only; no multi-def. Distinct from
 * gj_hwdb_ok_u_12568 (batch12568), gj_hwdb_ok_u_12368 (batch12368),
 * gj_hwdb_ok_u_12168 (batch12168), and peer lamps in this wave. Soft
 * stub — does not open hwdb.bin or query properties. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12768_marker[] = "libcgj-batch12768";

/* Soft unavailable / not-detected hwdb lamp. */
#define B12768_HWDB_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12768_hwdb_ok(void)
{
	return B12768_HWDB_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hwdb_ok_u_12768 - hwdb soft-id lamp.
 *
 * Always returns 0. Soft pure-data hwdb presence tag. Does not open
 * hwdb.bin or call libc. No parent wires.
 */
uint32_t
gj_hwdb_ok_u_12768(void)
{
	(void)NULL;
	return b12768_hwdb_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hwdb_ok_u_12768(void)
    __attribute__((alias("gj_hwdb_ok_u_12768")));
