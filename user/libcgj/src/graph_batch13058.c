/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13058: controller soft joydev ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joydev_ok_u_13058(void);
 *     - Returns 0 (soft stub: legacy joydev slot is soft/unprobed; not
 *       a runtime /dev/input/js* open or joydev ioctl probe).
 *   uint32_t __gj_joydev_ok_u_13058  (alias)
 *   __libcgj_batch13058_marker = "libcgj-batch13058"
 *
 * Exclusive continuum CREATE-ONLY (13051-13060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13060 —
 * controller_ok_u_13051, gamepad_ok_u_13052, joystick_ok_u_13053,
 * steam_input_ok_u_13054, hidraw_ok_u_13055, evdev_pad_ok_u_13056,
 * xpad_ok_u_13057, joydev_ok_u_13058, controller_soft_ready_u_13059,
 * batch_id_13060). Unique gj_joydev_ok_u_13058 surface only; no
 * multi-def. Distinct from gj_joydev_ok_u_12858 (batch12858),
 * gj_joydev_ok_u_12658 (batch12658), and sibling controller soft ok_u
 * stubs in this wave. No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13058_marker[] = "libcgj-batch13058";

/* Soft-stub joydev ok value (unprobed). */
#define B13058_JOYDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13058_joydev_ok(void)
{
	return B13058_JOYDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joydev_ok_u_13058 - soft joydev ok stub.
 *
 * Always returns 0 (soft/unprobed joydev slot). Does not call libc or
 * open /dev/input/js*. No parent wires.
 */
uint32_t
gj_joydev_ok_u_13058(void)
{
	(void)NULL;
	return b13058_joydev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joydev_ok_u_13058(void)
    __attribute__((alias("gj_joydev_ok_u_13058")));
