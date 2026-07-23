/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11264: input soft hidraw-ok lamp (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_11264(void);
 *     - Always returns 0. Soft stub only; intentionally not a live
 *       /dev/hidraw* presence probe for the input soft continuum.
 *   uint32_t __gj_hidraw_ok_u_11264  (alias)
 *   __libcgj_batch11264_marker = "libcgj-batch11264"
 *
 * Exclusive continuum CREATE-ONLY (11261-11270: input soft —
 * udev_ok_u_11261, libinput_ok_u_11262, evdev_ok_u_11263,
 * hidraw_ok_u_11264, uinput_ok_u_11265, hotplug_ok_u_11266,
 * device_db_ok_u_11267, hwdb_ok_u_11268, input_soft_ready_u_11269,
 * batch_id_11270). Unique surface only; no multi-def. Distinct from
 * gj_hidraw_ok_u_11064 / gj_hidraw_ok_u_10864 prior waves. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11264_marker[] = "libcgj-batch11264";

/* Soft hidraw-ok lamp: always off (not a real hidraw probe). */
#define B11264_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11264_hidraw_ok(void)
{
	return B11264_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_11264 - input soft hidraw-ok unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open hidraw nodes or
 * call libc. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_11264(void)
{
	(void)NULL;
	return b11264_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_11264(void)
    __attribute__((alias("gj_hidraw_ok_u_11264")));
