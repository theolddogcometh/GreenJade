/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5377: Wayland wl_shm XRGB8888 format enum (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_shm_format_xrgb_u(void);
 *     - Returns WL_SHM_FORMAT_XRGB8888 enum value (1).
 *   uint32_t __gj_wl_shm_format_xrgb_u  (alias)
 *   __libcgj_batch5377_marker = "libcgj-batch5377"
 *
 * Exclusive continuum CREATE-ONLY (5371-5380: X11/Wayland unique —
 * x11_success_u, x11_keypress_u, x11_buttonpress_u, x11_motion_u,
 * wl_display_ok_u, wl_compositor_v_u, wl_shm_format_xrgb_u,
 * wl_pointer_btn_left_u, wl_pointer_btn_right_u, batch_id_5380).
 * Unique gj_wl_shm_format_xrgb_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5377_marker[] = "libcgj-batch5377";

/* Wayland wl_shm pixel format: XRGB8888. */
#define B5377_WL_SHM_FORMAT_XRGB  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5377_wl_shm_format_xrgb(void)
{
	return B5377_WL_SHM_FORMAT_XRGB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_shm_format_xrgb_u - report the wl_shm XRGB8888 format enum.
 *
 * Always returns 1 (WL_SHM_FORMAT_XRGB8888). Soft compile-time constant
 * for freestanding shm pool setup; does not allocate buffers. No parent
 * wires.
 */
uint32_t
gj_wl_shm_format_xrgb_u(void)
{
	(void)NULL;
	return b5377_wl_shm_format_xrgb();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_shm_format_xrgb_u(void)
    __attribute__((alias("gj_wl_shm_format_xrgb_u")));
