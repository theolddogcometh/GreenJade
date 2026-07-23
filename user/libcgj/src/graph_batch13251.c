/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13251: controller soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_ok_u_13251(void);
 *     - Returns 0 (soft stub: generic controller stack slot is
 *       soft/unprobed; not a runtime gamepad/HID probe).
 *   uint32_t __gj_controller_ok_u_13251  (alias)
 *   __libcgj_batch13251_marker = "libcgj-batch13251"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_controller_ok_u_13251 surface only; no
 * multi-def. Distinct from gj_controller_ok_u_13051 (batch13051),
 * gj_controller_ok_u_12851 (batch12851), gj_controller_ok_u_12651
 * (batch12651), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13251_marker[] = "libcgj-batch13251";

/* Soft-stub controller ok value (unprobed). */
#define B13251_CONTROLLER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13251_controller_ok(void)
{
	return B13251_CONTROLLER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_ok_u_13251 - soft generic controller ok stub.
 *
 * Always returns 0 (soft/unprobed controller slot). Does not call libc
 * or probe gamepad/HID devices. No parent wires.
 */
uint32_t
gj_controller_ok_u_13251(void)
{
	(void)NULL;
	return b13251_controller_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_ok_u_13251(void)
    __attribute__((alias("gj_controller_ok_u_13251")));
