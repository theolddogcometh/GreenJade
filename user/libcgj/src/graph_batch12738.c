/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12738: IM soft keyboard-layout ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyboard_layout_ok_u_12738(void);
 *     - Returns 0 (soft stub: keyboard layout selection slot is
 *       soft/unprobed; not a runtime XKB/layout or setxkbmap probe).
 *   uint32_t __gj_keyboard_layout_ok_u_12738  (alias)
 *   __libcgj_batch12738_marker = "libcgj-batch12738"
 *
 * Exclusive continuum CREATE-ONLY (12731-12740: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→12740). Unique
 * gj_keyboard_layout_ok_u_12738 surface only; no multi-def. Distinct
 * from sibling IM soft ok_u stubs and prior xkb layout soft tags. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12738_marker[] = "libcgj-batch12738";

/* Soft-stub keyboard-layout ok value (unprobed). */
#define B12738_KEYBOARD_LAYOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12738_keyboard_layout_ok(void)
{
	return B12738_KEYBOARD_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyboard_layout_ok_u_12738 - soft keyboard-layout ok stub.
 *
 * Always returns 0 (soft/unprobed keyboard-layout slot). Does not call
 * libc or probe XKB layouts. No parent wires.
 */
uint32_t
gj_keyboard_layout_ok_u_12738(void)
{
	(void)NULL;
	return b12738_keyboard_layout_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyboard_layout_ok_u_12738(void)
    __attribute__((alias("gj_keyboard_layout_ok_u_12738")));
