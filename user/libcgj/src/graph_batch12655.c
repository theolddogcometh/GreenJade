/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12655: controller soft hidraw ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_12655(void);
 *     - Returns 0 (soft stub: hidraw controller slot is soft/unprobed;
 *       not a runtime /dev/hidraw* open or HID descriptor probe).
 *   uint32_t __gj_hidraw_ok_u_12655  (alias)
 *   __libcgj_batch12655_marker = "libcgj-batch12655"
 *
 * Exclusive continuum CREATE-ONLY (12651-12660: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12660 —
 * controller_ok_u_12651, gamepad_ok_u_12652, joystick_ok_u_12653,
 * steam_input_ok_u_12654, hidraw_ok_u_12655, evdev_pad_ok_u_12656,
 * xpad_ok_u_12657, joydev_ok_u_12658, controller_soft_ready_u_12659,
 * batch_id_12660). Unique gj_hidraw_ok_u_12655 surface only; no
 * multi-def. Distinct from gj_hidraw_ok_u_12455 (batch12455),
 * gj_hidraw_ok_u_12055 (batch12055), gj_hidraw_ok_u_11955
 * (batch11955), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12655_marker[] = "libcgj-batch12655";

/* Soft-stub hidraw ok value (unprobed). */
#define B12655_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12655_hidraw_ok(void)
{
	return B12655_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_12655 - soft hidraw ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw*. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_12655(void)
{
	(void)NULL;
	return b12655_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_12655(void)
    __attribute__((alias("gj_hidraw_ok_u_12655")));
