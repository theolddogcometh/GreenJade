/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11955: controller soft hidraw ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_11955(void);
 *     - Returns 0 (soft stub: hidraw controller slot is soft/unprobed;
 *       not a runtime /dev/hidraw* open or HID descriptor probe).
 *   uint32_t __gj_hidraw_ok_u_11955  (alias)
 *   __libcgj_batch11955_marker = "libcgj-batch11955"
 *
 * Exclusive continuum CREATE-ONLY (11951-11960: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→11960 —
 * controller_ok_u_11951, gamepad_ok_u_11952, joystick_ok_u_11953,
 * steam_input_ok_u_11954, hidraw_ok_u_11955, evdev_pad_ok_u_11956,
 * xpad_ok_u_11957, joydev_ok_u_11958, controller_soft_ready_u_11959,
 * batch_id_11960). Unique gj_hidraw_ok_u_11955 surface only; no
 * multi-def. Distinct from gj_hidraw_ok_u_11755 (batch11755),
 * gj_hidraw_ok_u_11555 (batch11555), and sibling controller soft ok_u
 * stubs in this wave. No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11955_marker[] = "libcgj-batch11955";

/* Soft-stub hidraw ok value (unprobed). */
#define B11955_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11955_hidraw_ok(void)
{
	return B11955_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_11955 - soft hidraw ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw*. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_11955(void)
{
	(void)NULL;
	return b11955_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_11955(void)
    __attribute__((alias("gj_hidraw_ok_u_11955")));
