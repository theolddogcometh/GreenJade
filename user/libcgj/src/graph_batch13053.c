/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13053: controller soft joystick ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joystick_ok_u_13053(void);
 *     - Returns 0 (soft stub: joystick slot is soft/unprobed; not a
 *       runtime joystick enumeration or /dev/input/js* probe).
 *   uint32_t __gj_joystick_ok_u_13053  (alias)
 *   __libcgj_batch13053_marker = "libcgj-batch13053"
 *
 * Exclusive continuum CREATE-ONLY (13051-13060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13060 —
 * controller_ok_u_13051, gamepad_ok_u_13052, joystick_ok_u_13053,
 * steam_input_ok_u_13054, hidraw_ok_u_13055, evdev_pad_ok_u_13056,
 * xpad_ok_u_13057, joydev_ok_u_13058, controller_soft_ready_u_13059,
 * batch_id_13060). Unique gj_joystick_ok_u_13053 surface only; no
 * multi-def. Distinct from gj_joystick_ok_u_12853 (batch12853),
 * gj_joystick_ok_u_12653 (batch12653), and sibling controller soft
 * ok_u stubs in this wave. No parent wires. No __int128. No
 * controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13053_marker[] = "libcgj-batch13053";

/* Soft-stub joystick ok value (unprobed). */
#define B13053_JOYSTICK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13053_joystick_ok(void)
{
	return B13053_JOYSTICK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joystick_ok_u_13053 - soft joystick ok stub.
 *
 * Always returns 0 (soft/unprobed joystick slot). Does not call libc or
 * probe joystick devices. No parent wires.
 */
uint32_t
gj_joystick_ok_u_13053(void)
{
	(void)NULL;
	return b13053_joystick_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joystick_ok_u_13053(void)
    __attribute__((alias("gj_joystick_ok_u_13053")));
