/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11238: IM soft keyboard-layout ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyboard_layout_ok_u_11238(void);
 *     - Returns 0 (soft stub: keyboard layout selection slot is
 *       soft/unprobed; not a runtime XKB/layout or setxkbmap probe).
 *   uint32_t __gj_keyboard_layout_ok_u_11238  (alias)
 *   __libcgj_batch11238_marker = "libcgj-batch11238"
 *
 * Exclusive continuum CREATE-ONLY (11231-11240: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→11240). Unique
 * gj_keyboard_layout_ok_u_11238 surface only; no multi-def. Distinct
 * from sibling IM soft ok_u stubs and prior xkb layout soft tags. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11238_marker[] = "libcgj-batch11238";

/* Soft-stub keyboard-layout ok value (unprobed). */
#define B11238_KEYBOARD_LAYOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11238_keyboard_layout_ok(void)
{
	return B11238_KEYBOARD_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyboard_layout_ok_u_11238 - soft keyboard-layout ok stub.
 *
 * Always returns 0 (soft/unprobed keyboard-layout slot). Does not call
 * libc or probe XKB layouts. No parent wires.
 */
uint32_t
gj_keyboard_layout_ok_u_11238(void)
{
	(void)NULL;
	return b11238_keyboard_layout_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyboard_layout_ok_u_11238(void)
    __attribute__((alias("gj_keyboard_layout_ok_u_11238")));
