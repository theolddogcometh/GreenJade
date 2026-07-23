/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13051: controller soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_controller_ok_u_13051(void);
 *     - Returns 0 (soft stub: generic controller stack slot is
 *       soft/unprobed; not a runtime gamepad/HID probe).
 *   uint32_t __gj_controller_ok_u_13051  (alias)
 *   __libcgj_batch13051_marker = "libcgj-batch13051"
 *
 * Exclusive continuum CREATE-ONLY (13051-13060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13060 —
 * controller_ok_u_13051, gamepad_ok_u_13052, joystick_ok_u_13053,
 * steam_input_ok_u_13054, hidraw_ok_u_13055, evdev_pad_ok_u_13056,
 * xpad_ok_u_13057, joydev_ok_u_13058, controller_soft_ready_u_13059,
 * batch_id_13060). Unique gj_controller_ok_u_13051 surface only; no
 * multi-def. Distinct from gj_controller_ok_u_12851 (batch12851),
 * gj_controller_ok_u_12651 (batch12651), gj_controller_ok_u_12451
 * (batch12451), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13051_marker[] = "libcgj-batch13051";

/* Soft-stub controller ok value (unprobed). */
#define B13051_CONTROLLER_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13051_controller_ok(void)
{
	return B13051_CONTROLLER_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_controller_ok_u_13051 - soft generic controller ok stub.
 *
 * Always returns 0 (soft/unprobed controller slot). Does not call libc
 * or probe gamepad/HID devices. No parent wires.
 */
uint32_t
gj_controller_ok_u_13051(void)
{
	(void)NULL;
	return b13051_controller_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_controller_ok_u_13051(void)
    __attribute__((alias("gj_controller_ok_u_13051")));
