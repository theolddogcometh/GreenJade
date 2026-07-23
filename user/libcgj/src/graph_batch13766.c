/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13766: keyboard soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kbd_ok_u_13766(void);
 *     - Returns 0 (soft stub: keyboard input slot is soft/unprobed;
 *       not a runtime keyboard or keymap probe).
 *   uint32_t __gj_kbd_ok_u_13766  (alias)
 *   __libcgj_batch13766_marker = "libcgj-batch13766"
 *
 * Exclusive continuum CREATE-ONLY (13761-13770: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13770 —
 * udev_ok_u_13761, libinput_ok_u_13762, evdev_ok_u_13763,
 * uinput_ok_u_13764, hidraw_ok_u_13765, kbd_ok_u_13766,
 * mouse_ok_u_13767, touch_ok_u_13768, input_soft_ready_u_13769,
 * batch_id_13770). Unique gj_kbd_ok_u_13766 surface only; no multi-def.
 * Distinct from gj_kbd_ok_u_13566 (batch13566), gj_kbd_ok_u_13366
 * (batch13366), and sibling input soft ok_u stubs in this wave. No
 * parent wires. No __int128. No keyboard implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13766_marker[] = "libcgj-batch13766";

/* Soft-stub keyboard ok value (unprobed). */
#define B13766_KBD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13766_kbd_ok(void)
{
	return B13766_KBD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kbd_ok_u_13766 - soft keyboard input ok stub.
 *
 * Always returns 0 (soft/unprobed keyboard slot). Does not call libc or
 * probe keyboards/keymaps. No parent wires.
 */
uint32_t
gj_kbd_ok_u_13766(void)
{
	(void)NULL;
	return b13766_kbd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kbd_ok_u_13766(void)
    __attribute__((alias("gj_kbd_ok_u_13766")));
