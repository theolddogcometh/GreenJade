/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13167: mouse/pointer soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mouse_ok_u_13167(void);
 *     - Returns 0 (soft stub: mouse/pointer input slot is soft/unprobed;
 *       not a runtime relative/absolute pointer probe).
 *   uint32_t __gj_mouse_ok_u_13167  (alias)
 *   __libcgj_batch13167_marker = "libcgj-batch13167"
 *
 * Exclusive continuum CREATE-ONLY (13161-13170: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13170 —
 * udev_ok_u_13161, libinput_ok_u_13162, evdev_ok_u_13163,
 * uinput_ok_u_13164, hidraw_ok_u_13165, kbd_ok_u_13166,
 * mouse_ok_u_13167, touch_ok_u_13168, input_soft_ready_u_13169,
 * batch_id_13170). Unique gj_mouse_ok_u_13167 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No mouse/pointer implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13167_marker[] = "libcgj-batch13167";

/* Soft-stub mouse ok value (unprobed). */
#define B13167_MOUSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13167_mouse_ok(void)
{
	return B13167_MOUSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_ok_u_13167 - soft mouse/pointer input ok stub.
 *
 * Always returns 0 (soft/unprobed mouse slot). Does not call libc or
 * probe pointer devices. No parent wires.
 */
uint32_t
gj_mouse_ok_u_13167(void)
{
	(void)NULL;
	return b13167_mouse_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mouse_ok_u_13167(void)
    __attribute__((alias("gj_mouse_ok_u_13167")));
