/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5376: Wayland wl_compositor interface version (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_compositor_v_u(void);
 *     - Returns the common wl_compositor interface version (4).
 *   uint32_t __gj_wl_compositor_v_u  (alias)
 *   __libcgj_batch5376_marker = "libcgj-batch5376"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_wl_compositor_v_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5376_marker[] = "libcgj-batch5376";

/* Wayland wl_compositor interface version (common bound version). */
#define B5376_WL_COMPOSITOR_V  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5376_wl_compositor_v(void)
{
	return B5376_WL_COMPOSITOR_V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_compositor_v_u - report the wl_compositor interface version.
 *
 * Always returns 4. Soft compile-time constant for freestanding Wayland
 * registry binding; does not connect a display. No parent wires.
 */
uint32_t
gj_wl_compositor_v_u(void)
{
	(void)NULL;
	return b5376_wl_compositor_v();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_compositor_v_u(void)
    __attribute__((alias("gj_wl_compositor_v_u")));
