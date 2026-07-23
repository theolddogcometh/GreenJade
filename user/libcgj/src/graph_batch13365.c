/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13365: hidraw soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hidraw_ok_u_13365(void);
 *     - Returns 0 (soft stub: HID raw device slot is soft/unprobed;
 *       not a runtime /dev/hidraw* probe).
 *   uint32_t __gj_hidraw_ok_u_13365  (alias)
 *   __libcgj_batch13365_marker = "libcgj-batch13365"
 *
 * Exclusive continuum CREATE-ONLY (13361-13370: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13370 —
 * udev_ok_u_13361, libinput_ok_u_13362, evdev_ok_u_13363,
 * uinput_ok_u_13364, hidraw_ok_u_13365, kbd_ok_u_13366,
 * mouse_ok_u_13367, touch_ok_u_13368, input_soft_ready_u_13369,
 * batch_id_13370). Unique gj_hidraw_ok_u_13365 surface only; no multi-def.
 * Distinct from gj_hidraw_ok_u_13165 (batch13165), sibling input soft
 * ok_u stubs in this wave, and controller-wave hidraw stubs. No parent
 * wires. No __int128. No HID implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13365_marker[] = "libcgj-batch13365";

/* Soft-stub hidraw ok value (unprobed). */
#define B13365_HIDRAW_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13365_hidraw_ok(void)
{
	return B13365_HIDRAW_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hidraw_ok_u_13365 - soft HID raw device ok stub.
 *
 * Always returns 0 (soft/unprobed hidraw slot). Does not call libc or
 * open /dev/hidraw nodes. No parent wires.
 */
uint32_t
gj_hidraw_ok_u_13365(void)
{
	(void)NULL;
	return b13365_hidraw_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hidraw_ok_u_13365(void)
    __attribute__((alias("gj_hidraw_ok_u_13365")));
