/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12968: touch/multitouch soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_touch_ok_u_12968(void);
 *     - Returns 0 (soft stub: touch/multitouch input slot is
 *       soft/unprobed; not a runtime touchscreen/touchpad probe).
 *   uint32_t __gj_touch_ok_u_12968  (alias)
 *   __libcgj_batch12968_marker = "libcgj-batch12968"
 *
 * Exclusive continuum CREATE-ONLY (12961-12970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→12970 —
 * udev_ok_u_12961, libinput_ok_u_12962, evdev_ok_u_12963,
 * uinput_ok_u_12964, hidraw_ok_u_12965, kbd_ok_u_12966,
 * mouse_ok_u_12967, touch_ok_u_12968, input_soft_ready_u_12969,
 * batch_id_12970). Unique gj_touch_ok_u_12968 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No touch implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12968_marker[] = "libcgj-batch12968";

/* Soft-stub touch ok value (unprobed). */
#define B12968_TOUCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12968_touch_ok(void)
{
	return B12968_TOUCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_touch_ok_u_12968 - soft touch/multitouch input ok stub.
 *
 * Always returns 0 (soft/unprobed touch slot). Does not call libc or
 * probe touchscreens/touchpads. No parent wires.
 */
uint32_t
gj_touch_ok_u_12968(void)
{
	(void)NULL;
	return b12968_touch_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_touch_ok_u_12968(void)
    __attribute__((alias("gj_touch_ok_u_12968")));
