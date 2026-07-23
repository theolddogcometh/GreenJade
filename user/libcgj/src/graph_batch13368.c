/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13368: touch/multitouch soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_touch_ok_u_13368(void);
 *     - Returns 0 (soft stub: touch/multitouch input slot is
 *       soft/unprobed; not a runtime touchscreen/touchpad probe).
 *   uint32_t __gj_touch_ok_u_13368  (alias)
 *   __libcgj_batch13368_marker = "libcgj-batch13368"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_touch_ok_u_13368 surface only; no multi-def.
 * Distinct from gj_touch_ok_u_13168 (batch13168) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No touch
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13368_marker[] = "libcgj-batch13368";

/* Soft-stub touch ok value (unprobed). */
#define B13368_TOUCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13368_touch_ok(void)
{
	return B13368_TOUCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_touch_ok_u_13368 - soft touch/multitouch input ok stub.
 *
 * Always returns 0 (soft/unprobed touch slot). Does not call libc or
 * probe touchscreens/touchpads. No parent wires.
 */
uint32_t
gj_touch_ok_u_13368(void)
{
	(void)NULL;
	return b13368_touch_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_touch_ok_u_13368(void)
    __attribute__((alias("gj_touch_ok_u_13368")));
