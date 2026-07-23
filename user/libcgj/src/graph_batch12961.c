/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12961: udev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_udev_ok_u_12961(void);
 *     - Returns 0 (soft stub: udev/device enumeration stack slot is
 *       soft/unprobed; not a runtime udev or sysfs probe).
 *   uint32_t __gj_udev_ok_u_12961  (alias)
 *   __libcgj_batch12961_marker = "libcgj-batch12961"
 *
 * Exclusive continuum CREATE-ONLY (12961-12970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→12970 —
 * udev_ok_u_12961, libinput_ok_u_12962, evdev_ok_u_12963,
 * uinput_ok_u_12964, hidraw_ok_u_12965, kbd_ok_u_12966,
 * mouse_ok_u_12967, touch_ok_u_12968, input_soft_ready_u_12969,
 * batch_id_12970). Unique gj_udev_ok_u_12961 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No udev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12961_marker[] = "libcgj-batch12961";

/* Soft-stub udev ok value (unprobed). */
#define B12961_UDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12961_udev_ok(void)
{
	return B12961_UDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udev_ok_u_12961 - soft udev/device enumeration ok stub.
 *
 * Always returns 0 (soft/unprobed udev slot). Does not call libc or
 * probe sysfs/udev. No parent wires.
 */
uint32_t
gj_udev_ok_u_12961(void)
{
	(void)NULL;
	return b12961_udev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_udev_ok_u_12961(void)
    __attribute__((alias("gj_udev_ok_u_12961")));
