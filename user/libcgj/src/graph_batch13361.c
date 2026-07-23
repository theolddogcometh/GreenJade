/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13361: udev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_ok_u_13361(void);
 *     - Returns 0 (soft stub: udev/device enumeration stack slot is
 *       soft/unprobed; not a runtime udev or sysfs probe).
 *   uint32_t __gj_udev_ok_u_13361  (alias)
 *   __libcgj_batch13361_marker = "libcgj-batch13361"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_udev_ok_u_13361 surface only; no multi-def.
 * Distinct from gj_udev_ok_u_13161 (batch13161) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No udev
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13361_marker[] = "libcgj-batch13361";

/* Soft-stub udev ok value (unprobed). */
#define B13361_UDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13361_udev_ok(void)
{
	return B13361_UDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_ok_u_13361 - soft udev/device enumeration ok stub.
 *
 * Always returns 0 (soft/unprobed udev slot). Does not call libc or
 * probe sysfs/udev. No parent wires.
 */
uint32_t
gj_udev_ok_u_13361(void)
{
	(void)NULL;
	return b13361_udev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_ok_u_13361(void)
    __attribute__((alias("gj_udev_ok_u_13361")));
