/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13965: hidraw soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_13965(void);
 *     - Returns 0 (soft stub: HID raw device slot is soft/unprobed;
 *       not a runtime /dev/hidraw* probe).
 *   uint32_t __gj_hidraw_ok_u_13965  (alias)
 *   __libcgj_batch13965_marker = "libcgj-batch13965"
 *
 * Exclusive continuum CREATE-ONLY (13961-13970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13970 —
 * udev_ok_u_13961, libinput_ok_u_13962, evdev_ok_u_13963,
 * uinput_ok_u_13964, hidraw_ok_u_13965, kbd_ok_u_13966,
 * mouse_ok_u_13967, touch_ok_u_13968, input_soft_ready_u_13969,
 * batch_id_13970). Unique gj_hidraw_ok_u_13965 surface only; no multi-def.
 * Distinct from gj_hidraw_ok_u_13765 (batch13765), gj_hidraw_ok_u_13565
 * (batch13565), gj_hidraw_ok_u_13365 (batch13365), and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No hidraw
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13965_marker[] = "libcgj-batch13965";

/* Soft-stub hidraw ok value (unprobed). */
#define B13965_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13965_hidraw_ok(void)
{
	return B13965_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_13965 - soft HID raw device ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw nodes. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_13965(void)
{
	(void)NULL;
	return b13965_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_13965(void)
    __attribute__((alias("gj_hidraw_ok_u_13965")));
