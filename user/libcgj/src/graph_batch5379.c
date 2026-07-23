/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5379: Wayland/Linux BTN_RIGHT pointer code (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_pointer_btn_right_u(void);
 *     - Returns the Linux input BTN_RIGHT code used by wl_pointer (0x111).
 *   uint32_t __gj_wl_pointer_btn_right_u  (alias)
 *   __libcgj_batch5379_marker = "libcgj-batch5379"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_wl_pointer_btn_right_u surface only; no multi-def. Distinct
 * from gj_hid_btn_right_u (batch5322). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5379_marker[] = "libcgj-batch5379";

/* Linux input / wl_pointer button code: BTN_RIGHT. */
#define B5379_WL_POINTER_BTN_RIGHT  0x111u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5379_wl_pointer_btn_right(void)
{
	return B5379_WL_POINTER_BTN_RIGHT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_pointer_btn_right_u - report the wl_pointer right button code.
 *
 * Always returns 0x111 (BTN_RIGHT). Soft compile-time constant for
 * freestanding Wayland pointer button packing; does not probe input
 * devices. No parent wires.
 */
uint32_t
gj_wl_pointer_btn_right_u(void)
{
	(void)NULL;
	return b5379_wl_pointer_btn_right();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_pointer_btn_right_u(void)
    __attribute__((alias("gj_wl_pointer_btn_right_u")));
