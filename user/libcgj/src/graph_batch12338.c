/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12338: keyboard layout soft identity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyboard_layout_ok_u_12338(void);
 *     - Returns 0 (soft: keyboard layout path not ready at compile time).
 *   uint32_t __gj_keyboard_layout_ok_u_12338  (alias)
 *   __libcgj_batch12338_marker = "libcgj-batch12338"
 *
 * Exclusive CREATE-ONLY wave 12331-12340 (IM soft: ibus/fcitx soft id
 * stubs — ready→1 others 0). Unique gj_keyboard_layout_ok_u_12338
 * surface only; no multi-def. Distinct from
 * gj_keyboard_layout_ok_u_12138 (batch12138),
 * gj_keyboard_layout_ok_u_11838 (batch11838), and sibling IM soft stubs
 * in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12338_marker[] = "libcgj-batch12338";

/* Soft keyboard layout ok lamp: not ready at compile time. */
#define B12338_KEYBOARD_LAYOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12338_ok(void)
{
	return B12338_KEYBOARD_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyboard_layout_ok_u_12338 - soft keyboard layout ok lamp.
 *
 * Always returns 0 (not ready at compile time). Does not query XKB
 * layouts or call libc. No parent wires.
 */
uint32_t
gj_keyboard_layout_ok_u_12338(void)
{
	(void)NULL;
	return b12338_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyboard_layout_ok_u_12338(void)
    __attribute__((alias("gj_keyboard_layout_ok_u_12338")));
