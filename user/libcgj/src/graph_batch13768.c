/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13768: touch/multitouch soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_touch_ok_u_13768(void);
 *     - Returns 0 (soft stub: touch/multitouch input slot is
 *       soft/unprobed; not a runtime touchscreen/touchpad probe).
 *   uint32_t __gj_touch_ok_u_13768  (alias)
 *   __libcgj_batch13768_marker = "libcgj-batch13768"
 *
 * Exclusive continuum CREATE-ONLY (13761-13770: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13770 —
 * udev_ok_u_13761, libinput_ok_u_13762, evdev_ok_u_13763,
 * uinput_ok_u_13764, hidraw_ok_u_13765, kbd_ok_u_13766,
 * mouse_ok_u_13767, touch_ok_u_13768, input_soft_ready_u_13769,
 * batch_id_13770). Unique gj_touch_ok_u_13768 surface only; no multi-def.
 * Distinct from gj_touch_ok_u_13568 (batch13568), gj_touch_ok_u_13368
 * (batch13368), and sibling input soft ok_u stubs in this wave. No
 * parent wires. No __int128. No touch implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13768_marker[] = "libcgj-batch13768";

/* Soft-stub touch ok value (unprobed). */
#define B13768_TOUCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13768_touch_ok(void)
{
	return B13768_TOUCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_touch_ok_u_13768 - soft touch/multitouch input ok stub.
 *
 * Always returns 0 (soft/unprobed touch slot). Does not call libc or
 * probe touchscreens/touchpads. No parent wires.
 */
uint32_t
gj_touch_ok_u_13768(void)
{
	(void)NULL;
	return b13768_touch_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_touch_ok_u_13768(void)
    __attribute__((alias("gj_touch_ok_u_13768")));
