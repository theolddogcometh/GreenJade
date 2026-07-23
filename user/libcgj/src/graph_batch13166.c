/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13166: keyboard soft ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kbd_ok_u_13166(void);
 *     - Returns 0 (soft stub: keyboard input slot is soft/unprobed;
 *       not a runtime keyboard or keymap probe).
 *   uint32_t __gj_kbd_ok_u_13166  (alias)
 *   __libcgj_batch13166_marker = "libcgj-batch13166"
 *
 * Exclusive continuum CREATE-ONLY (13161-13170: input soft stubs —
 * ok slots all→0; input_soft_ready→1; batch_id→13170 —
 * udev_ok_u_13161, libinput_ok_u_13162, evdev_ok_u_13163,
 * uinput_ok_u_13164, hidraw_ok_u_13165, kbd_ok_u_13166,
 * mouse_ok_u_13167, touch_ok_u_13168, input_soft_ready_u_13169,
 * batch_id_13170). Unique gj_kbd_ok_u_13166 surface only; no multi-def.
 * Distinct from sibling input soft ok_u stubs in this wave. No parent
 * wires. No __int128. No keyboard implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13166_marker[] = "libcgj-batch13166";

/* Soft-stub keyboard ok value (unprobed). */
#define B13166_KBD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13166_kbd_ok(void)
{
	return B13166_KBD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kbd_ok_u_13166 - soft keyboard input ok stub.
 *
 * Always returns 0 (soft/unprobed keyboard slot). Does not call libc or
 * probe keyboards/keymaps. No parent wires.
 */
uint32_t
gj_kbd_ok_u_13166(void)
{
	(void)NULL;
	return b13166_kbd_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kbd_ok_u_13166(void)
    __attribute__((alias("gj_kbd_ok_u_13166")));
