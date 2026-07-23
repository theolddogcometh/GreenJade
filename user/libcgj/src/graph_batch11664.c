/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11664: input soft hidraw-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_11664(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       /dev/hidraw* presence probe for the input soft continuum.
 *   uint32_t __gj_hidraw_ok_u_11664  (alias)
 *   __libcgj_batch11664_marker = "libcgj-batch11664"
 *
 * Exclusive continuum CREATE-ONLY (11661-11670: input soft —
 * udev_ok_u_11661, libinput_ok_u_11662, evdev_ok_u_11663,
 * hidraw_ok_u_11664, uinput_ok_u_11665, hotplug_ok_u_11666,
 * device_db_ok_u_11667, hwdb_ok_u_11668, input_soft_ready_u_11669,
 * batch_id_11670). Unique surface only; no multi-def. Distinct from
 * gj_hidraw_ok_u_11464 / gj_hidraw_ok_u_11264 prior waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11664_marker[] = "libcgj-batch11664";

/* Soft hidraw-ok lamp: always off (not a real hidraw probe). */
#define B11664_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11664_hidraw_ok(void)
{
	return B11664_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_11664 - input soft hidraw-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open /dev/hidraw*,
 * enumerate HID devices, or call libc. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_11664(void)
{
	(void)NULL;
	return b11664_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_11664(void)
    __attribute__((alias("gj_hidraw_ok_u_11664")));
