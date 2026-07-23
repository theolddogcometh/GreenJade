/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12458: controller soft joydev ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_joydev_ok_u_12458(void);
 *     - Returns 0 (soft stub: legacy joydev slot is soft/unprobed; not
 *       a runtime /dev/input/js* open or joydev ioctl probe).
 *   uint32_t __gj_joydev_ok_u_12458  (alias)
 *   __libcgj_batch12458_marker = "libcgj-batch12458"
 *
 * Exclusive continuum CREATE-ONLY (12451-12460: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12460 —
 * controller_ok_u_12451, gamepad_ok_u_12452, joystick_ok_u_12453,
 * steam_input_ok_u_12454, hidraw_ok_u_12455, evdev_pad_ok_u_12456,
 * xpad_ok_u_12457, joydev_ok_u_12458, controller_soft_ready_u_12459,
 * batch_id_12460). Unique gj_joydev_ok_u_12458 surface only; no
 * multi-def. Distinct from gj_joydev_ok_u_12058 (batch12058),
 * gj_joydev_ok_u_11958 (batch11958), gj_joydev_ok_u_11758
 * (batch11758), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12458_marker[] = "libcgj-batch12458";

/* Soft-stub joydev ok value (unprobed). */
#define B12458_JOYDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12458_joydev_ok(void)
{
	return B12458_JOYDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_joydev_ok_u_12458 - soft joydev ok stub.
 *
 * Always returns 0 (soft/unprobed joydev slot). Does not call libc or
 * open /dev/input/js*. No parent wires.
 */
uint32_t
gj_joydev_ok_u_12458(void)
{
	(void)NULL;
	return b12458_joydev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_joydev_ok_u_12458(void)
    __attribute__((alias("gj_joydev_ok_u_12458")));
