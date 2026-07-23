/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12057: controller soft xpad ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xpad_ok_u_12057(void);
 *     - Returns 0 (soft stub: xpad/Xbox controller slot is soft/unprobed;
 *       not a runtime xpad module or Xbox HID probe).
 *   uint32_t __gj_xpad_ok_u_12057  (alias)
 *   __libcgj_batch12057_marker = "libcgj-batch12057"
 *
 * Exclusive continuum CREATE-ONLY (12051-12060: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12060 —
 * controller_ok_u_12051, gamepad_ok_u_12052, joystick_ok_u_12053,
 * steam_input_ok_u_12054, hidraw_ok_u_12055, evdev_pad_ok_u_12056,
 * xpad_ok_u_12057, joydev_ok_u_12058, controller_soft_ready_u_12059,
 * batch_id_12060). Unique gj_xpad_ok_u_12057 surface only; no
 * multi-def. Distinct from gj_xpad_ok_u_11957 (batch11957),
 * gj_xpad_ok_u_11757 (batch11757), gj_xpad_ok_u_11557 (batch11557),
 * and sibling controller soft ok_u stubs in this wave. No parent
 * wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12057_marker[] = "libcgj-batch12057";

/* Soft-stub xpad ok value (unprobed). */
#define B12057_XPAD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12057_xpad_ok(void)
{
	return B12057_XPAD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xpad_ok_u_12057 - soft xpad ok stub.
 *
 * Always returns 0 (soft/unprobed xpad slot). Does not call libc or
 * probe Xbox/xpad devices. No parent wires.
 */
uint32_t
gj_xpad_ok_u_12057(void)
{
	(void)NULL;
	return b12057_xpad_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xpad_ok_u_12057(void)
    __attribute__((alias("gj_xpad_ok_u_12057")));
