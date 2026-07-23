/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12855: controller soft hidraw ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_12855(void);
 *     - Returns 0 (soft stub: hidraw controller slot is soft/unprobed;
 *       not a runtime /dev/hidraw* open or HID descriptor probe).
 *   uint32_t __gj_hidraw_ok_u_12855  (alias)
 *   __libcgj_batch12855_marker = "libcgj-batch12855"
 *
 * Exclusive continuum CREATE-ONLY (12851-12860: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12860 —
 * controller_ok_u_12851, gamepad_ok_u_12852, joystick_ok_u_12853,
 * steam_input_ok_u_12854, hidraw_ok_u_12855, evdev_pad_ok_u_12856,
 * xpad_ok_u_12857, joydev_ok_u_12858, controller_soft_ready_u_12859,
 * batch_id_12860). Unique gj_hidraw_ok_u_12855 surface only; no
 * multi-def. Distinct from gj_hidraw_ok_u_11955 (batch11955),
 * gj_hidraw_ok_u_11755 (batch11755), and sibling controller soft ok_u
 * stubs in this wave. No parent wires. No __int128. No controller
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12855_marker[] = "libcgj-batch12855";

/* Soft-stub hidraw ok value (unprobed). */
#define B12855_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12855_hidraw_ok(void)
{
	return B12855_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_12855 - soft hidraw ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw*. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_12855(void)
{
	(void)NULL;
	return b12855_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_12855(void)
    __attribute__((alias("gj_hidraw_ok_u_12855")));
