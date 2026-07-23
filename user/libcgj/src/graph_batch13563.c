/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13563: evdev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_13563(void);
 *     - Returns 0 (soft stub: evdev input device slot is soft/unprobed;
 *       not a runtime /dev/input/event* probe).
 *   uint32_t __gj_evdev_ok_u_13563  (alias)
 *   __libcgj_batch13563_marker = "libcgj-batch13563"
 *
 * Exclusive continuum CREATE-ONLY (13561-13570: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13570 —
 * udev_ok_u_13561, libinput_ok_u_13562, evdev_ok_u_13563,
 * uinput_ok_u_13564, hidraw_ok_u_13565, kbd_ok_u_13566,
 * mouse_ok_u_13567, touch_ok_u_13568, input_soft_ready_u_13569,
 * batch_id_13570). Unique gj_evdev_ok_u_13563 surface only; no multi-def.
 * Distinct from gj_evdev_ok_u_13363 (batch13363) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No evdev
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13563_marker[] = "libcgj-batch13563";

/* Soft-stub evdev ok value (unprobed). */
#define B13563_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13563_evdev_ok(void)
{
	return B13563_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_13563 - soft evdev input device ok stub.
 *
 * Always returns 0 (soft/unprobed evdev slot). Does not call libc or
 * open /dev/input/event nodes. No parent wires.
 */
uint32_t
gj_evdev_ok_u_13563(void)
{
	(void)NULL;
	return b13563_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_13563(void)
    __attribute__((alias("gj_evdev_ok_u_13563")));
