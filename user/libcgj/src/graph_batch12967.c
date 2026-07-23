/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12967: mouse/pointer soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mouse_ok_u_12967(void);
 *     - Returns 0 (soft stub: mouse/pointer input slot is soft/unprobed;
 *       not a runtime relative/absolute pointer probe).
 *   uint32_t __gj_mouse_ok_u_12967  (alias)
 *   __libcgj_batch12967_marker = "libcgj-batch12967"
 *
 * Exclusive continuum CREATE-ONLY (12961-12970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→12970 —
 * udev_ok_u_12961, libinput_ok_u_12962, evdev_ok_u_12963,
 * uinput_ok_u_12964, hidraw_ok_u_12965, kbd_ok_u_12966,
 * mouse_ok_u_12967, touch_ok_u_12968, input_soft_ready_u_12969,
 * batch_id_12970). Unique gj_mouse_ok_u_12967 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No mouse/pointer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12967_marker[] = "libcgj-batch12967";

/* Soft-stub mouse ok value (unprobed). */
#define B12967_MOUSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12967_mouse_ok(void)
{
	return B12967_MOUSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_ok_u_12967 - soft mouse/pointer input ok stub.
 *
 * Always returns 0 (soft/unprobed mouse slot). Does not call libc or
 * probe pointer devices. No parent wires.
 */
uint32_t
gj_mouse_ok_u_12967(void)
{
	(void)NULL;
	return b12967_mouse_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mouse_ok_u_12967(void)
    __attribute__((alias("gj_mouse_ok_u_12967")));
