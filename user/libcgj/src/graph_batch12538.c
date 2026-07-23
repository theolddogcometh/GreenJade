/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12538: keyboard layout soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyboard_layout_ok_u_12538(void);
 *     - Returns 0 (soft: keyboard layout path not ready at compile time).
 *   uint32_t __gj_keyboard_layout_ok_u_12538  (alias)
 *   __libcgj_batch12538_marker = "libcgj-batch12538"
 *
 * Exclusive CREATE-ONLY wave 12531-12540 (ibus/fcitx soft id stubs).
 * Unique gj_keyboard_layout_ok_u_12538 surface only; no multi-def. No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12538_marker[] = "libcgj-batch12538";

/* Soft keyboard layout ok lamp: not ready at compile time. */
#define B12538_KEYBOARD_LAYOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12538_ok(void)
{
	return B12538_KEYBOARD_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyboard_layout_ok_u_12538 - soft keyboard layout ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not query XKB
 * layouts or call libc. No parent wires.
 */
uint32_t
gj_keyboard_layout_ok_u_12538(void)
{
	(void)NULL;
	return b12538_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyboard_layout_ok_u_12538(void)
    __attribute__((alias("gj_keyboard_layout_ok_u_12538")));
