/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13966: keyboard soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kbd_ok_u_13966(void);
 *     - Returns 0 (soft stub: keyboard input slot is soft/unprobed;
 *       not a runtime keyboard or keymap probe).
 *   uint32_t __gj_kbd_ok_u_13966  (alias)
 *   __libcgj_batch13966_marker = "libcgj-batch13966"
 *
 * Exclusive continuum CREATE-ONLY (13961-13970: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13970 —
 * udev_ok_u_13961, libinput_ok_u_13962, evdev_ok_u_13963,
 * uinput_ok_u_13964, hidraw_ok_u_13965, kbd_ok_u_13966,
 * mouse_ok_u_13967, touch_ok_u_13968, input_soft_ready_u_13969,
 * batch_id_13970). Unique gj_kbd_ok_u_13966 surface only; no multi-def.
 * Distinct from gj_kbd_ok_u_13766 (batch13766), gj_kbd_ok_u_13566
 * (batch13566), gj_kbd_ok_u_13366 (batch13366), and sibling input soft
 * ok_u stubs in this wave. No parent wires. No __int128. No keyboard
 * implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13966_marker[] = "libcgj-batch13966";

/* Soft-stub keyboard ok value (unprobed). */
#define B13966_KBD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13966_kbd_ok(void)
{
	return B13966_KBD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kbd_ok_u_13966 - soft keyboard input ok stub.
 *
 * Always returns 0 (soft/unprobed keyboard slot). Does not call libc or
 * probe keyboards/keymaps. No parent wires.
 */
uint32_t
gj_kbd_ok_u_13966(void)
{
	(void)NULL;
	return b13966_kbd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kbd_ok_u_13966(void)
    __attribute__((alias("gj_kbd_ok_u_13966")));
