/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12963: evdev soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_evdev_ok_u_12963(void);
 *     - Returns 0 (soft stub: Linux evdev input node slot is
 *       soft/unprobed; not a runtime /dev/input/event* probe).
 *   uint32_t __gj_evdev_ok_u_12963  (alias)
 *   __libcgj_batch12963_marker = "libcgj-batch12963"
 *
 * Exclusive continuum CREATE-ONLY (12961-12970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→12970 —
 * udev_ok_u_12961, libinput_ok_u_12962, evdev_ok_u_12963,
 * uinput_ok_u_12964, hidraw_ok_u_12965, kbd_ok_u_12966,
 * mouse_ok_u_12967, touch_ok_u_12968, input_soft_ready_u_12969,
 * batch_id_12970). Unique gj_evdev_ok_u_12963 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No evdev implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12963_marker[] = "libcgj-batch12963";

/* Soft-stub evdev ok value (unprobed). */
#define B12963_EVDEV_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12963_evdev_ok(void)
{
	return B12963_EVDEV_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_evdev_ok_u_12963 - soft Linux evdev input ok stub.
 *
 * Always returns 0 (soft/unprobed evdev slot). Does not call libc or
 * open /dev/input nodes. No parent wires.
 */
uint32_t
gj_evdev_ok_u_12963(void)
{
	(void)NULL;
	return b12963_evdev_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_evdev_ok_u_12963(void)
    __attribute__((alias("gj_evdev_ok_u_12963")));
