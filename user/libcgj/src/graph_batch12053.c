/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12053: controller soft joystick ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joystick_ok_u_12053(void);
 *     - Returns 0 (soft stub: joystick slot is soft/unprobed; not a
 *       runtime joystick enumeration or /dev/input/js* probe).
 *   uint32_t __gj_joystick_ok_u_12053  (alias)
 *   __libcgj_batch12053_marker = "libcgj-batch12053"
 *
 * Exclusive continuum CREATE-ONLY (12051-12060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12060 —
 * controller_ok_u_12051, gamepad_ok_u_12052, joystick_ok_u_12053,
 * steam_input_ok_u_12054, hidraw_ok_u_12055, evdev_pad_ok_u_12056,
 * xpad_ok_u_12057, joydev_ok_u_12058, controller_soft_ready_u_12059,
 * batch_id_12060). Unique gj_joystick_ok_u_12053 surface only; no
 * multi-def. Distinct from gj_joystick_ok_u_11953 (batch11953),
 * gj_joystick_ok_u_11753 (batch11753), gj_joystick_ok_u_11553
 * (batch11553), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12053_marker[] = "libcgj-batch12053";

/* Soft-stub joystick ok value (unprobed). */
#define B12053_JOYSTICK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12053_joystick_ok(void)
{
	return B12053_JOYSTICK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joystick_ok_u_12053 - soft joystick ok stub.
 *
 * Always returns 0 (soft/unprobed joystick slot). Does not call libc or
 * probe joystick devices. No parent wires.
 */
uint32_t
gj_joystick_ok_u_12053(void)
{
	(void)NULL;
	return b12053_joystick_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joystick_ok_u_12053(void)
    __attribute__((alias("gj_joystick_ok_u_12053")));
