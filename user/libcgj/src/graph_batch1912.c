/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1912: right mouse button code predicate.
 *
 * Surface (unique symbols):
 *   int gj_mouse_btn_right_p(int code);
 *     — Return 1 if code is the classic Linux EV_KEY right mouse
 *       button (BTN_RIGHT = 0x111), else 0.
 *   int __gj_mouse_btn_right_p  (alias)
 *   __libcgj_batch1912_marker = "libcgj-batch1912"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Local constants only; no linux/input.h. Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1912_marker[] = "libcgj-batch1912";

/* Linux input-event-codes: BTN_RIGHT. */
#define B1912_BTN_RIGHT  0x111

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code equals BTN_RIGHT (0x111). */
static int
b1912_is_right(int nCode)
{
	return (nCode == B1912_BTN_RIGHT) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_btn_right_p — 1 if code is BTN_RIGHT (0x111).
 *
 * code: EV_KEY button code (raw int; no input.h)
 * Returns 1 for right button, else 0.
 */
int
gj_mouse_btn_right_p(int nCode)
{
	(void)NULL;
	return b1912_is_right(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mouse_btn_right_p(int nCode)
    __attribute__((alias("gj_mouse_btn_right_p")));
