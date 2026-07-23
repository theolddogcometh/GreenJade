/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13767: mouse/pointer soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mouse_ok_u_13767(void);
 *     - Returns 0 (soft stub: mouse/pointer input slot is soft/unprobed;
 *       not a runtime pointer device probe).
 *   uint32_t __gj_mouse_ok_u_13767  (alias)
 *   __libcgj_batch13767_marker = "libcgj-batch13767"
 *
 * Exclusive continuum CREATE-ONLY (13761-13770: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13770 —
 * udev_ok_u_13761, libinput_ok_u_13762, evdev_ok_u_13763,
 * uinput_ok_u_13764, hidraw_ok_u_13765, kbd_ok_u_13766,
 * mouse_ok_u_13767, touch_ok_u_13768, input_soft_ready_u_13769,
 * batch_id_13770). Unique gj_mouse_ok_u_13767 surface only; no multi-def.
 * Distinct from gj_mouse_ok_u_13567 (batch13567), gj_mouse_ok_u_13367
 * (batch13367), and sibling input soft ok_u stubs in this wave. No
 * parent wires. No __int128. No mouse implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13767_marker[] = "libcgj-batch13767";

/* Soft-stub mouse/pointer ok value (unprobed). */
#define B13767_MOUSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13767_mouse_ok(void)
{
	return B13767_MOUSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_ok_u_13767 - soft mouse/pointer input ok stub.
 *
 * Always returns 0 (soft/unprobed mouse slot). Does not call libc or
 * probe pointer devices. No parent wires.
 */
uint32_t
gj_mouse_ok_u_13767(void)
{
	(void)NULL;
	return b13767_mouse_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mouse_ok_u_13767(void)
    __attribute__((alias("gj_mouse_ok_u_13767")));
