/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13258: controller soft joydev ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joydev_ok_u_13258(void);
 *     - Returns 0 (soft stub: legacy joydev slot is soft/unprobed; not
 *       a runtime /dev/input/js* open or joydev ioctl probe).
 *   uint32_t __gj_joydev_ok_u_13258  (alias)
 *   __libcgj_batch13258_marker = "libcgj-batch13258"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_joydev_ok_u_13258 surface only; no
 * multi-def. Distinct from gj_joydev_ok_u_13058 (batch13058),
 * gj_joydev_ok_u_12858 (batch12858), gj_joydev_ok_u_12658
 * (batch12658), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13258_marker[] = "libcgj-batch13258";

/* Soft-stub joydev ok value (unprobed). */
#define B13258_JOYDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13258_joydev_ok(void)
{
	return B13258_JOYDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joydev_ok_u_13258 - soft joydev ok stub.
 *
 * Always returns 0 (soft/unprobed joydev slot). Does not call libc or
 * open /dev/input/js*. No parent wires.
 */
uint32_t
gj_joydev_ok_u_13258(void)
{
	(void)NULL;
	return b13258_joydev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joydev_ok_u_13258(void)
    __attribute__((alias("gj_joydev_ok_u_13258")));
