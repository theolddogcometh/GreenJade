/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1848: arrow keycode hint predicate.
 *
 * Surface (unique symbols):
 *   int gj_keycode_is_arrow_hint(uint32_t code);
 *     — Return 1 if code matches a conventional Linux input-event
 *       arrow keycode (KEY_UP=103, KEY_LEFT=105, KEY_RIGHT=106,
 *       KEY_DOWN=108), else 0. Hint only — other ABIs may differ.
 *   int __gj_keycode_is_arrow_hint  (alias)
 *   __libcgj_batch1848_marker = "libcgj-batch1848"
 *
 * Keyboard/input exclusive pure-data wave (1841–1850).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1848_marker[] = "libcgj-batch1848";

/* Linux input-event-codes.h KEY_* arrow values (classic AT keyboard). */
#define B1848_KEY_UP    103u
#define B1848_KEY_LEFT  105u
#define B1848_KEY_RIGHT 106u
#define B1848_KEY_DOWN  108u

/* ---- freestanding helpers ---------------------------------------------- */

/* True when uCode is one of the four classic arrow KEY_* values. */
static int
b1848_is_arrow(uint32_t uCode)
{
	if (uCode == B1848_KEY_UP) {
		return 1;
	}
	if (uCode == B1848_KEY_LEFT) {
		return 1;
	}
	if (uCode == B1848_KEY_RIGHT) {
		return 1;
	}
	if (uCode == B1848_KEY_DOWN) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_keycode_is_arrow_hint — 1 if code is a Linux KEY_* arrow.
 *
 * uCode: keycode in the Linux input-event numbering scheme
 * Returns 1 for 103/105/106/108, else 0. Pure local hint.
 */
int
gj_keycode_is_arrow_hint(uint32_t uCode)
{
	return b1848_is_arrow(uCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_keycode_is_arrow_hint(uint32_t uCode)
    __attribute__((alias("gj_keycode_is_arrow_hint")));
