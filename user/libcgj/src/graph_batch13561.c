/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13561: udev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_ok_u_13561(void);
 *     - Returns 0 (soft stub: udev/device enumeration stack slot is
 *       soft/unprobed; not a runtime udev or sysfs probe).
 *   uint32_t __gj_udev_ok_u_13561  (alias)
 *   __libcgj_batch13561_marker = "libcgj-batch13561"
 *
 * Exclusive continuum CREATE-ONLY (13561-13570: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13570 —
 * udev_ok_u_13561, libinput_ok_u_13562, evdev_ok_u_13563,
 * uinput_ok_u_13564, hidraw_ok_u_13565, kbd_ok_u_13566,
 * mouse_ok_u_13567, touch_ok_u_13568, input_soft_ready_u_13569,
 * batch_id_13570). Unique gj_udev_ok_u_13561 surface only; no multi-def.
 * Distinct from gj_udev_ok_u_13361 (batch13361) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No udev
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13561_marker[] = "libcgj-batch13561";

/* Soft-stub udev ok value (unprobed). */
#define B13561_UDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13561_udev_ok(void)
{
	return B13561_UDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_ok_u_13561 - soft udev/device enumeration ok stub.
 *
 * Always returns 0 (soft/unprobed udev slot). Does not call libc or
 * probe sysfs/udev. No parent wires.
 */
uint32_t
gj_udev_ok_u_13561(void)
{
	(void)NULL;
	return b13561_udev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_ok_u_13561(void)
    __attribute__((alias("gj_udev_ok_u_13561")));
