/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1911: left mouse button code predicate.
 *
 * Surface (unique symbols):
 *   int gj_mouse_btn_left_p(int code);
 *     — Return 1 if code is the classic Linux EV_KEY left mouse
 *       button (BTN_LEFT = 0x110), else 0.
 *   int __gj_mouse_btn_left_p  (alias)
 *   __libcgj_batch1911_marker = "libcgj-batch1911"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Local constants only; no linux/input.h. Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1911_marker[] = "libcgj-batch1911";

/* Linux input-event-codes: BTN_LEFT. */
#define B1911_BTN_LEFT  0x110

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code equals BTN_LEFT (0x110). */
static int
b1911_is_left(int nCode)
{
	return (nCode == B1911_BTN_LEFT) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_btn_left_p — 1 if code is BTN_LEFT (0x110).
 *
 * code: EV_KEY button code (raw int; no input.h)
 * Returns 1 for left button, else 0.
 */
int
gj_mouse_btn_left_p(int nCode)
{
	(void)NULL;
	return b1911_is_left(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mouse_btn_left_p(int nCode)
    __attribute__((alias("gj_mouse_btn_left_p")));
