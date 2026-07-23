/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12564: hidraw soft-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_12564(void);
 *     - Soft hidraw presence lamp (always 0 — not detected /
 *       soft-unavailable). Compile-time pure-data tag; not a live
 *       /dev/hidraw* probe.
 *   uint32_t __gj_hidraw_ok_u_12564  (alias)
 *   __libcgj_batch12564_marker = "libcgj-batch12564"
 *
 * Exclusive continuum CREATE-ONLY (12561-12570: udev/libinput soft id
 * stubs — udev_ok, libinput_ok, evdev_ok, hidraw_ok, uinput_ok,
 * hotplug_ok, device_db_ok, hwdb_ok, input_soft_ready, batch_id_12570).
 * Unique gj_hidraw_ok_u_12564 surface only; no multi-def. Distinct from
 * gj_hidraw_ok_u_12364 (batch12364), gj_hidraw_ok_u_12164 (batch12164),
 * and peer lamps in this wave. Soft stub — does not open hidraw nodes
 * or read report descriptors. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12564_marker[] = "libcgj-batch12564";

/* Soft unavailable / not-detected hidraw lamp. */
#define B12564_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12564_hidraw_ok(void)
{
	return B12564_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_12564 - hidraw soft-id lamp.
 *
 * Always returns 0. Soft pure-data hidraw presence tag. Does not probe
 * /dev/hidraw or call libc. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_12564(void)
{
	(void)NULL;
	return b12564_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_12564(void)
    __attribute__((alias("gj_hidraw_ok_u_12564")));
