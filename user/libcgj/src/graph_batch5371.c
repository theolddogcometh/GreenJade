/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5371: X11 Success status code constant (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_x11_success_u(void);
 *     - Returns the X11 protocol Success status code (0).
 *   uint32_t __gj_x11_success_u  (alias)
 *   __libcgj_batch5371_marker = "libcgj-batch5371"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_x11_success_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5371_marker[] = "libcgj-batch5371";

/* X11 protocol status: Success. */
#define B5371_X11_SUCCESS  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5371_x11_success(void)
{
	return B5371_X11_SUCCESS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_x11_success_u - report the X11 Success status code.
 *
 * Always returns 0. Soft compile-time constant for freestanding X11
 * status packing; does not open a display. No parent wires.
 */
uint32_t
gj_x11_success_u(void)
{
	(void)NULL;
	return b5371_x11_success();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_x11_success_u(void)
    __attribute__((alias("gj_x11_success_u")));
