/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13763: evdev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_13763(void);
 *     - Returns 0 (soft stub: evdev input device slot is soft/unprobed;
 *       not a runtime /dev/input/event* probe).
 *   uint32_t __gj_evdev_ok_u_13763  (alias)
 *   __libcgj_batch13763_marker = "libcgj-batch13763"
 *
 * Exclusive continuum CREATE-ONLY (13761-13770: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13770 —
 * udev_ok_u_13761, libinput_ok_u_13762, evdev_ok_u_13763,
 * uinput_ok_u_13764, hidraw_ok_u_13765, kbd_ok_u_13766,
 * mouse_ok_u_13767, touch_ok_u_13768, input_soft_ready_u_13769,
 * batch_id_13770). Unique gj_evdev_ok_u_13763 surface only; no multi-def.
 * Distinct from gj_evdev_ok_u_13563 (batch13563), gj_evdev_ok_u_13363
 * (batch13363), and sibling input soft ok_u stubs in this wave. No
 * parent wires. No __int128. No evdev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13763_marker[] = "libcgj-batch13763";

/* Soft-stub evdev ok value (unprobed). */
#define B13763_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13763_evdev_ok(void)
{
	return B13763_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_13763 - soft evdev input device ok stub.
 *
 * Always returns 0 (soft/unprobed evdev slot). Does not call libc or
 * open /dev/input/event nodes. No parent wires.
 */
uint32_t
gj_evdev_ok_u_13763(void)
{
	(void)NULL;
	return b13763_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_13763(void)
    __attribute__((alias("gj_evdev_ok_u_13763")));
