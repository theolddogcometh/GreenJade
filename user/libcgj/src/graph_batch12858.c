/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12858: controller soft joydev ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joydev_ok_u_12858(void);
 *     - Returns 0 (soft stub: legacy joydev slot is soft/unprobed; not
 *       a runtime /dev/input/js* open or joydev ioctl probe).
 *   uint32_t __gj_joydev_ok_u_12858  (alias)
 *   __libcgj_batch12858_marker = "libcgj-batch12858"
 *
 * Exclusive continuum CREATE-ONLY (12851-12860: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12860 —
 * controller_ok_u_12851, gamepad_ok_u_12852, joystick_ok_u_12853,
 * steam_input_ok_u_12854, hidraw_ok_u_12855, evdev_pad_ok_u_12856,
 * xpad_ok_u_12857, joydev_ok_u_12858, controller_soft_ready_u_12859,
 * batch_id_12860). Unique gj_joydev_ok_u_12858 surface only; no
 * multi-def. Distinct from gj_joydev_ok_u_11958 (batch11958),
 * gj_joydev_ok_u_11758 (batch11758), and sibling controller soft ok_u
 * stubs in this wave. No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12858_marker[] = "libcgj-batch12858";

/* Soft-stub joydev ok value (unprobed). */
#define B12858_JOYDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12858_joydev_ok(void)
{
	return B12858_JOYDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joydev_ok_u_12858 - soft joydev ok stub.
 *
 * Always returns 0 (soft/unprobed joydev slot). Does not call libc or
 * open /dev/input/js*. No parent wires.
 */
uint32_t
gj_joydev_ok_u_12858(void)
{
	(void)NULL;
	return b12858_joydev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joydev_ok_u_12858(void)
    __attribute__((alias("gj_joydev_ok_u_12858")));
