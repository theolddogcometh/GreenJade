/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13938: keyboard layout soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyboard_layout_ok_u_13938(void);
 *     - Returns 0 (soft: keyboard layout path not ready at compile time).
 *   uint32_t __gj_keyboard_layout_ok_u_13938  (alias)
 *   __libcgj_batch13938_marker = "libcgj-batch13938"
 *
 * Exclusive CREATE-ONLY wave 13931-13940 (ibus/fcitx soft id stubs).
 * Unique gj_keyboard_layout_ok_u_13938 surface only; no multi-def. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13938_marker[] = "libcgj-batch13938";

/* Soft keyboard layout ok lamp: not ready at compile time. */
#define B13938_KEYBOARD_LAYOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13938_ok(void)
{
	return B13938_KEYBOARD_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyboard_layout_ok_u_13938 - soft keyboard layout ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not query XKB
 * layouts or call libc. No parent wires.
 */
uint32_t
gj_keyboard_layout_ok_u_13938(void)
{
	(void)NULL;
	return b13938_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyboard_layout_ok_u_13938(void)
    __attribute__((alias("gj_keyboard_layout_ok_u_13938")));
