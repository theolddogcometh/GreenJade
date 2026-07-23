/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11958: controller soft joydev ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joydev_ok_u_11958(void);
 *     - Returns 0 (soft stub: legacy joydev slot is soft/unprobed; not
 *       a runtime /dev/input/js* open or joydev ioctl probe).
 *   uint32_t __gj_joydev_ok_u_11958  (alias)
 *   __libcgj_batch11958_marker = "libcgj-batch11958"
 *
 * Exclusive continuum CREATE-ONLY (11951-11960: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11960 —
 * controller_ok_u_11951, gamepad_ok_u_11952, joystick_ok_u_11953,
 * steam_input_ok_u_11954, hidraw_ok_u_11955, evdev_pad_ok_u_11956,
 * xpad_ok_u_11957, joydev_ok_u_11958, controller_soft_ready_u_11959,
 * batch_id_11960). Unique gj_joydev_ok_u_11958 surface only; no
 * multi-def. Distinct from gj_joydev_ok_u_11758 (batch11758),
 * gj_joydev_ok_u_11558 (batch11558), and sibling controller soft ok_u
 * stubs in this wave. No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11958_marker[] = "libcgj-batch11958";

/* Soft-stub joydev ok value (unprobed). */
#define B11958_JOYDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11958_joydev_ok(void)
{
	return B11958_JOYDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joydev_ok_u_11958 - soft joydev ok stub.
 *
 * Always returns 0 (soft/unprobed joydev slot). Does not call libc or
 * open /dev/input/js*. No parent wires.
 */
uint32_t
gj_joydev_ok_u_11958(void)
{
	(void)NULL;
	return b11958_joydev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joydev_ok_u_11958(void)
    __attribute__((alias("gj_joydev_ok_u_11958")));
