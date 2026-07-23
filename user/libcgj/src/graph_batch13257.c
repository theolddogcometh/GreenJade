/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13257: controller soft xpad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_ok_u_13257(void);
 *     - Returns 0 (soft stub: xpad/Xbox controller slot is soft/unprobed;
 *       not a runtime xpad module or Xbox HID probe).
 *   uint32_t __gj_xpad_ok_u_13257  (alias)
 *   __libcgj_batch13257_marker = "libcgj-batch13257"
 *
 * Exclusive continuum CREATE-ONLY (13251-13260: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→13260 —
 * controller_ok_u_13251, gamepad_ok_u_13252, joystick_ok_u_13253,
 * steam_input_ok_u_13254, hidraw_ok_u_13255, evdev_pad_ok_u_13256,
 * xpad_ok_u_13257, joydev_ok_u_13258, controller_soft_ready_u_13259,
 * batch_id_13260). Unique gj_xpad_ok_u_13257 surface only; no
 * multi-def. Distinct from gj_xpad_ok_u_13057 (batch13057),
 * gj_xpad_ok_u_12857 (batch12857), gj_xpad_ok_u_12657 (batch12657),
 * and sibling controller soft ok_u stubs in this wave. No parent
 * wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13257_marker[] = "libcgj-batch13257";

/* Soft-stub xpad ok value (unprobed). */
#define B13257_XPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13257_xpad_ok(void)
{
	return B13257_XPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_ok_u_13257 - soft xpad ok stub.
 *
 * Always returns 0 (soft/unprobed xpad slot). Does not call libc or
 * probe Xbox/xpad devices. No parent wires.
 */
uint32_t
gj_xpad_ok_u_13257(void)
{
	(void)NULL;
	return b13257_xpad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_ok_u_13257(void)
    __attribute__((alias("gj_xpad_ok_u_13257")));
