/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5378: Wayland/Linux BTN_LEFT pointer code (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_pointer_btn_left_u(void);
 *     - Returns the Linux input BTN_LEFT code used by wl_pointer (0x110).
 *   uint32_t __gj_wl_pointer_btn_left_u  (alias)
 *   __libcgj_batch5378_marker = "libcgj-batch5378"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_wl_pointer_btn_left_u surface only; no multi-def. Distinct
 * from gj_hid_btn_left_u (batch5321). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5378_marker[] = "libcgj-batch5378";

/* Linux input / wl_pointer button code: BTN_LEFT. */
#define B5378_WL_POINTER_BTN_LEFT  0x110u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5378_wl_pointer_btn_left(void)
{
	return B5378_WL_POINTER_BTN_LEFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_pointer_btn_left_u - report the wl_pointer left button code.
 *
 * Always returns 0x110 (BTN_LEFT). Soft compile-time constant for
 * freestanding Wayland pointer button packing; does not probe input
 * devices. No parent wires.
 */
uint32_t
gj_wl_pointer_btn_left_u(void)
{
	(void)NULL;
	return b5378_wl_pointer_btn_left();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_pointer_btn_left_u(void)
    __attribute__((alias("gj_wl_pointer_btn_left_u")));
