/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11265: input soft uinput-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_uinput_ok_u_11265(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       /dev/uinput presence probe for the input soft continuum.
 *   uint32_t __gj_uinput_ok_u_11265  (alias)
 *   __libcgj_batch11265_marker = "libcgj-batch11265"
 *
 * Exclusive continuum CREATE-ONLY (11261-11270: input soft —
 * udev_ok_u_11261, libinput_ok_u_11262, evdev_ok_u_11263,
 * hidraw_ok_u_11264, uinput_ok_u_11265, hotplug_ok_u_11266,
 * device_db_ok_u_11267, hwdb_ok_u_11268, input_soft_ready_u_11269,
 * batch_id_11270). Unique surface only; no multi-def. Distinct from
 * gj_uinput_ok_u_11065 / gj_uinput_ok_u_10865 prior waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11265_marker[] = "libcgj-batch11265";

/* Soft uinput-ok lamp: always off (not a real uinput probe). */
#define B11265_UINPUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11265_uinput_ok(void)
{
	return B11265_UINPUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uinput_ok_u_11265 - input soft uinput-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open /dev/uinput or
 * call libc. No parent wires.
 */
uint32_t
gj_uinput_ok_u_11265(void)
{
	(void)NULL;
	return b11265_uinput_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uinput_ok_u_11265(void)
    __attribute__((alias("gj_uinput_ok_u_11265")));
