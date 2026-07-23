/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1913: middle mouse button code predicate.
 *
 * Surface (unique symbols):
 *   int gj_mouse_btn_middle_p(int code);
 *     — Return 1 if code is the classic Linux EV_KEY middle mouse
 *       button (BTN_MIDDLE = 0x112), else 0.
 *   int __gj_mouse_btn_middle_p  (alias)
 *   __libcgj_batch1913_marker = "libcgj-batch1913"
 *
 * Input-device exclusive pure-data wave (1911–1919; milestone 1920).
 * Local constants only; no linux/input.h. Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1913_marker[] = "libcgj-batch1913";

/* Linux input-event-codes: BTN_MIDDLE. */
#define B1913_BTN_MIDDLE  0x112

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if code equals BTN_MIDDLE (0x112). */
static int
b1913_is_middle(int nCode)
{
	return (nCode == B1913_BTN_MIDDLE) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mouse_btn_middle_p — 1 if code is BTN_MIDDLE (0x112).
 *
 * code: EV_KEY button code (raw int; no input.h)
 * Returns 1 for middle button, else 0.
 */
int
gj_mouse_btn_middle_p(int nCode)
{
	(void)NULL;
	return b1913_is_middle(nCode);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mouse_btn_middle_p(int nCode)
    __attribute__((alias("gj_mouse_btn_middle_p")));
