/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5375: Wayland display connect-ok flag (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_display_ok_u(void);
 *     - Returns the freestanding Wayland display-ok success flag (1).
 *   uint32_t __gj_wl_display_ok_u  (alias)
 *   __libcgj_batch5375_marker = "libcgj-batch5375"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_wl_display_ok_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5375_marker[] = "libcgj-batch5375";

/* Wayland display soft-status: connected / ok. */
#define B5375_WL_DISPLAY_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5375_wl_display_ok(void)
{
	return B5375_WL_DISPLAY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_display_ok_u - report the Wayland display-ok success flag.
 *
 * Always returns 1. Soft compile-time constant for freestanding Wayland
 * status packing; does not connect a display. No parent wires.
 */
uint32_t
gj_wl_display_ok_u(void)
{
	(void)NULL;
	return b5375_wl_display_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_display_ok_u(void)
    __attribute__((alias("gj_wl_display_ok_u")));
