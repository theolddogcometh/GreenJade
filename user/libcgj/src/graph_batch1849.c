/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1849: function-key keycode hint predicate.
 *
 * Surface (unique symbols):
 *   int gj_keycode_is_function_hint(uint32_t code);
 *     — Return 1 if code matches a conventional Linux input-event
 *       function keycode: KEY_F1..KEY_F10 (59..68), KEY_F11 (87),
 *       KEY_F12 (88). Else 0. Hint only — other ABIs may differ.
 *   int __gj_keycode_is_function_hint  (alias)
 *   __libcgj_batch1849_marker = "libcgj-batch1849"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850). Distinct from
 * gj_keycode_is_arrow_hint (batch1848). Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1849_marker[] = "libcgj-batch1849";

/* Linux input-event-codes.h KEY_F1..F10 contiguous; F11/F12 separate. */
#define B1849_KEY_F1  59u
#define B1849_KEY_F10 68u
#define B1849_KEY_F11 87u
#define B1849_KEY_F12 88u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when uCode is KEY_F1..F10 or KEY_F11/F12. */
static int
b1849_is_function(uint32_t uCode)
{
	if (uCode >= B1849_KEY_F1 && uCode <= B1849_KEY_F10) {
		return 1;
	}
	if (uCode == B1849_KEY_F11) {
		return 1;
	}
	if (uCode == B1849_KEY_F12) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keycode_is_function_hint — 1 if code is a Linux KEY_Fn (1..12).
 *
 * uCode: keycode in the Linux input-event numbering scheme
 * Returns 1 for 59..68, 87, 88; else 0. Pure local hint.
 */
int
gj_keycode_is_function_hint(uint32_t uCode)
{
	return b1849_is_function(uCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_keycode_is_function_hint(uint32_t uCode)
    __attribute__((alias("gj_keycode_is_function_hint")));
