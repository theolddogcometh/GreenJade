/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5437: wl_seat capability pointer bit.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wl_seat_cap_pointer_5437(void);
 *     - Returns WL_SEAT_CAPABILITY_POINTER (1).
 *   uint32_t __gj_wl_seat_cap_pointer_5437  (alias)
 *   __libcgj_batch5437_marker = "libcgj-batch5437"
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

const char __libcgj_batch5437_marker[] = "libcgj-batch5437";

/* Wayland wl_seat capability: pointer. */
#define B5437_WL_SEAT_CAP_POINTER  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5437_wl_seat_cap_pointer(void)
{
	return B5437_WL_SEAT_CAP_POINTER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wl_seat_cap_pointer_5437 - report wl_seat pointer capability bit.
 *
 * Always returns 1 (WL_SEAT_CAPABILITY_POINTER). Soft compile-time
 * constant for freestanding seat capability packing; does not bind a
 * seat. No parent wires.
 */
uint32_t
gj_wl_seat_cap_pointer_5437(void)
{
	(void)NULL;
	return b5437_wl_seat_cap_pointer();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wl_seat_cap_pointer_5437(void)
    __attribute__((alias("gj_wl_seat_cap_pointer_5437")));
