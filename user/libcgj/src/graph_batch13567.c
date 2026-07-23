/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13567: mouse/pointer soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mouse_ok_u_13567(void);
 *     - Returns 0 (soft stub: mouse/pointer input slot is soft/unprobed;
 *       not a runtime pointer device probe).
 *   uint32_t __gj_mouse_ok_u_13567  (alias)
 *   __libcgj_batch13567_marker = "libcgj-batch13567"
 *
 * Exclusive continuum CREATE-ONLY (13561-13570: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13570 —
 * udev_ok_u_13561, libinput_ok_u_13562, evdev_ok_u_13563,
 * uinput_ok_u_13564, hidraw_ok_u_13565, kbd_ok_u_13566,
 * mouse_ok_u_13567, touch_ok_u_13568, input_soft_ready_u_13569,
 * batch_id_13570). Unique gj_mouse_ok_u_13567 surface only; no multi-def.
 * Distinct from gj_mouse_ok_u_13367 (batch13367) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No mouse
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13567_marker[] = "libcgj-batch13567";

/* Soft-stub mouse/pointer ok value (unprobed). */
#define B13567_MOUSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13567_mouse_ok(void)
{
	return B13567_MOUSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_ok_u_13567 - soft mouse/pointer input ok stub.
 *
 * Always returns 0 (soft/unprobed mouse slot). Does not call libc or
 * probe pointer devices. No parent wires.
 */
uint32_t
gj_mouse_ok_u_13567(void)
{
	(void)NULL;
	return b13567_mouse_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mouse_ok_u_13567(void)
    __attribute__((alias("gj_mouse_ok_u_13567")));
