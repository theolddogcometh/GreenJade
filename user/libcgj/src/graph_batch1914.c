/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1914: side mouse button code predicate.
 *
 * Surface (unique symbols):
 *   int gj_mouse_btn_side_p(int code);
 *     — Return 1 if code is the classic Linux EV_KEY side mouse
 *       button (BTN_SIDE = 0x113), else 0.
 *   int __gj_mouse_btn_side_p  (alias)
 *   __libcgj_batch1914_marker = "libcgj-batch1914"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Local constants only; no linux/input.h. Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1914_marker[] = "libcgj-batch1914";

/* Linux input-event-codes: BTN_SIDE. */
#define B1914_BTN_SIDE  0x113

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code equals BTN_SIDE (0x113). */
static int
b1914_is_side(int nCode)
{
	return (nCode == B1914_BTN_SIDE) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_btn_side_p — 1 if code is BTN_SIDE (0x113).
 *
 * code: EV_KEY button code (raw int; no input.h)
 * Returns 1 for side button, else 0.
 */
int
gj_mouse_btn_side_p(int nCode)
{
	(void)NULL;
	return b1914_is_side(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mouse_btn_side_p(int nCode)
    __attribute__((alias("gj_mouse_btn_side_p")));
