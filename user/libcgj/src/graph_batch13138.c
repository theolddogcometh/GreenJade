/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13138: IM soft keyboard-layout ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_keyboard_layout_ok_u_13138(void);
 *     - Returns 0 (soft stub: keyboard layout selection slot is
 *       soft/unprobed; not a runtime XKB/layout or setxkbmap probe).
 *   uint32_t __gj_keyboard_layout_ok_u_13138  (alias)
 *   __libcgj_batch13138_marker = "libcgj-batch13138"
 *
 * Exclusive continuum CREATE-ONLY (13131-13140: IM soft stubs —
 * ok slots all→0; im_soft_ready→1; batch_id→13140 —
 * ibus_ok_u_13131, fcitx_ok_u_13132, xkbcommon_ok_u_13133,
 * compose_ok_u_13134, im_module_ok_u_13135, locale_im_ok_u_13136,
 * input_method_ok_u_13137, keyboard_layout_ok_u_13138,
 * im_soft_ready_u_13139, batch_id_13140). Unique
 * gj_keyboard_layout_ok_u_13138 surface only; no multi-def. Distinct
 * from sibling IM soft ok_u stubs, prior xkb layout soft tags, and
 * gj_keyboard_layout_ok_u_12938 (batch12938) /
 * gj_keyboard_layout_ok_u_10438 (batch10438). No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13138_marker[] = "libcgj-batch13138";

/* Soft-stub keyboard-layout ok value (unprobed). */
#define B13138_KEYBOARD_LAYOUT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13138_keyboard_layout_ok(void)
{
	return B13138_KEYBOARD_LAYOUT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keyboard_layout_ok_u_13138 - soft keyboard-layout ok stub.
 *
 * Always returns 0 (soft/unprobed keyboard-layout slot). Does not call
 * libc or probe XKB layouts. No parent wires.
 */
uint32_t
gj_keyboard_layout_ok_u_13138(void)
{
	(void)NULL;
	return b13138_keyboard_layout_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_keyboard_layout_ok_u_13138(void)
    __attribute__((alias("gj_keyboard_layout_ok_u_13138")));
