/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13168: touch/multitouch soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_touch_ok_u_13168(void);
 *     - Returns 0 (soft stub: touch/multitouch input slot is
 *       soft/unprobed; not a runtime touchscreen/touchpad probe).
 *   uint32_t __gj_touch_ok_u_13168  (alias)
 *   __libcgj_batch13168_marker = "libcgj-batch13168"
 *
 * Exclusive continuum CREATE-ONLY (13161-13170: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13170 —
 * udev_ok_u_13161, libinput_ok_u_13162, evdev_ok_u_13163,
 * uinput_ok_u_13164, hidraw_ok_u_13165, kbd_ok_u_13166,
 * mouse_ok_u_13167, touch_ok_u_13168, input_soft_ready_u_13169,
 * batch_id_13170). Unique gj_touch_ok_u_13168 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No touch implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13168_marker[] = "libcgj-batch13168";

/* Soft-stub touch ok value (unprobed). */
#define B13168_TOUCH_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13168_touch_ok(void)
{
	return B13168_TOUCH_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_touch_ok_u_13168 - soft touch/multitouch input ok stub.
 *
 * Always returns 0 (soft/unprobed touch slot). Does not call libc or
 * probe touchscreens/touchpads. No parent wires.
 */
uint32_t
gj_touch_ok_u_13168(void)
{
	(void)NULL;
	return b13168_touch_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_touch_ok_u_13168(void)
    __attribute__((alias("gj_touch_ok_u_13168")));
