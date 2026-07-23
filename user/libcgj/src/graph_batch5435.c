/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5435: XCB connection-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_xcb_connection_ok_5435(void);
 *     - Returns freestanding xcb_connection success flag (1).
 *   uint32_t __gj_xcb_connection_ok_5435  (alias)
 *   __libcgj_batch5435_marker = "libcgj-batch5435"
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

const char __libcgj_batch5435_marker[] = "libcgj-batch5435";

/* Soft status: xcb_connect path green (no has_error). */
#define B5435_XCB_CONNECTION_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5435_xcb_connection_ok(void)
{
	return B5435_XCB_CONNECTION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xcb_connection_ok_5435 - report xcb connection success flag.
 *
 * Always returns 1. Soft compile-time constant for freestanding XCB
 * connection packing; does not call xcb_connect. No parent wires.
 */
uint32_t
gj_xcb_connection_ok_5435(void)
{
	(void)NULL;
	return b5435_xcb_connection_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xcb_connection_ok_5435(void)
    __attribute__((alias("gj_xcb_connection_ok_5435")));
