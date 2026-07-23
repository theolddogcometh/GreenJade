/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5432: X11 protocol major version constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_x11_display_proto_major_5432(void);
 *     - Returns the X11 protocol major version (11).
 *   uint32_t __gj_x11_display_proto_major_5432  (alias)
 *   __libcgj_batch5432_marker = "libcgj-batch5432"
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

const char __libcgj_batch5432_marker[] = "libcgj-batch5432";

/* X11 wire protocol major version (X11). */
#define B5432_X11_PROTO_MAJOR  11u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5432_x11_display_proto_major(void)
{
	return B5432_X11_PROTO_MAJOR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_x11_display_proto_major_5432 - report X11 protocol major version.
 *
 * Always returns 11. Soft compile-time constant for freestanding X11
 * connection setup packing; does not negotiate with a server.
 * No parent wires.
 */
uint32_t
gj_x11_display_proto_major_5432(void)
{
	(void)NULL;
	return b5432_x11_display_proto_major();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_x11_display_proto_major_5432(void)
    __attribute__((alias("gj_x11_display_proto_major_5432")));
