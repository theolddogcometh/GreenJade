/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12455: controller soft hidraw ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_12455(void);
 *     - Returns 0 (soft stub: hidraw controller slot is soft/unprobed;
 *       not a runtime /dev/hidraw* open or HID descriptor probe).
 *   uint32_t __gj_hidraw_ok_u_12455  (alias)
 *   __libcgj_batch12455_marker = "libcgj-batch12455"
 *
 * Exclusive continuum CREATE-ONLY (12451-12460: controller soft stubs —
 * ok slots all→0; controller_soft_ready→1; batch_id→12460 —
 * controller_ok_u_12451, gamepad_ok_u_12452, joystick_ok_u_12453,
 * steam_input_ok_u_12454, hidraw_ok_u_12455, evdev_pad_ok_u_12456,
 * xpad_ok_u_12457, joydev_ok_u_12458, controller_soft_ready_u_12459,
 * batch_id_12460). Unique gj_hidraw_ok_u_12455 surface only; no
 * multi-def. Distinct from gj_hidraw_ok_u_12055 (batch12055),
 * gj_hidraw_ok_u_11955 (batch11955), gj_hidraw_ok_u_11755
 * (batch11755), and sibling controller soft ok_u stubs in this wave.
 * No parent wires. No __int128. No controller implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12455_marker[] = "libcgj-batch12455";

/* Soft-stub hidraw ok value (unprobed). */
#define B12455_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12455_hidraw_ok(void)
{
	return B12455_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_12455 - soft hidraw ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw*. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_12455(void)
{
	(void)NULL;
	return b12455_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_12455(void)
    __attribute__((alias("gj_hidraw_ok_u_12455")));
