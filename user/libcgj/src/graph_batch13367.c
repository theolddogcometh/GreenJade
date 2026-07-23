/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13367: mouse/pointer soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mouse_ok_u_13367(void);
 *     - Returns 0 (soft stub: mouse/pointer input slot is soft/unprobed;
 *       not a runtime relative/absolute pointer probe).
 *   uint32_t __gj_mouse_ok_u_13367  (alias)
 *   __libcgj_batch13367_marker = "libcgj-batch13367"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_mouse_ok_u_13367 surface only; no multi-def.
 * Distinct from gj_mouse_ok_u_13167 (batch13167) and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No mouse/pointer
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13367_marker[] = "libcgj-batch13367";

/* Soft-stub mouse ok value (unprobed). */
#define B13367_MOUSE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13367_mouse_ok(void)
{
	return B13367_MOUSE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_ok_u_13367 - soft mouse/pointer input ok stub.
 *
 * Always returns 0 (soft/unprobed mouse slot). Does not call libc or
 * probe pointer devices. No parent wires.
 */
uint32_t
gj_mouse_ok_u_13367(void)
{
	(void)NULL;
	return b13367_mouse_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mouse_ok_u_13367(void)
    __attribute__((alias("gj_mouse_ok_u_13367")));
