/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5372: X11 KeyPress event type constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_x11_keypress_u(void);
 *     - Returns the X11 KeyPress event type code (2).
 *   uint32_t __gj_x11_keypress_u  (alias)
 *   __libcgj_batch5372_marker = "libcgj-batch5372"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_x11_keypress_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5372_marker[] = "libcgj-batch5372";

/* X11 event type: KeyPress. */
#define B5372_X11_KEYPRESS  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5372_x11_keypress(void)
{
	return B5372_X11_KEYPRESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_x11_keypress_u - report the X11 KeyPress event type code.
 *
 * Always returns 2. Soft compile-time constant for freestanding X11
 * event dispatch tables; does not open a display. No parent wires.
 */
uint32_t
gj_x11_keypress_u(void)
{
	(void)NULL;
	return b5372_x11_keypress();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_x11_keypress_u(void)
    __attribute__((alias("gj_x11_keypress_u")));
