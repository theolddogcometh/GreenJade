/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5436: XCB setup roots_len stub (one screen).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xcb_setup_roots_len_5436(void);
 *     - Returns freestanding xcb_setup roots_len (1 screen).
 *   uint32_t __gj_xcb_setup_roots_len_5436  (alias)
 *   __libcgj_batch5436_marker = "libcgj-batch5436"
 *
 * Exclusive continuum CREATE-ONLY (5431-5440: X11/Wayland display connect,
 * wl_display, xcb connection stubs, input seat, pointer/keyboard
 * capabilities for product shell —
 * x11_display_connect_ok_5431, x11_display_proto_major_5432,
 * wl_display_connect_ok_5433, wl_display_fd_stub_5434,
 * xcb_connection_ok_5435, xcb_setup_roots_len_5436,
 * wl_seat_cap_pointer_5437, wl_seat_cap_keyboard_5438,
 * shell_ptr_kbd_caps_5439, batch_id_5440). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5436_marker[] = "libcgj-batch5436";

/* Conventional single-root screen count for product shell stubs. */
#define B5436_XCB_SETUP_ROOTS_LEN  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5436_xcb_setup_roots_len(void)
{
	return B5436_XCB_SETUP_ROOTS_LEN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xcb_setup_roots_len_5436 - report xcb_setup roots_len.
 *
 * Always returns 1. Soft compile-time constant for freestanding XCB
 * screen iteration; does not parse a real setup blob. No parent wires.
 */
uint32_t
gj_xcb_setup_roots_len_5436(void)
{
	(void)NULL;
	return b5436_xcb_setup_roots_len();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xcb_setup_roots_len_5436(void)
    __attribute__((alias("gj_xcb_setup_roots_len_5436")));
