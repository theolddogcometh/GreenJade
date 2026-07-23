/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5439: product-shell pointer|keyboard caps mask.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shell_ptr_kbd_caps_5439(void);
 *     - Returns pointer|keyboard seat caps mask (1|2 = 3).
 *   uint32_t __gj_shell_ptr_kbd_caps_5439  (alias)
 *   __libcgj_batch5439_marker = "libcgj-batch5439"
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

const char __libcgj_batch5439_marker[] = "libcgj-batch5439";

/* Product-shell seat mask: POINTER (1) | KEYBOARD (2). */
#define B5439_SHELL_PTR_KBD_CAPS  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5439_shell_ptr_kbd_caps(void)
{
	return B5439_SHELL_PTR_KBD_CAPS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shell_ptr_kbd_caps_5439 - report product-shell input caps mask.
 *
 * Always returns 3 (pointer|keyboard). Soft compile-time constant for
 * freestanding product shell seat readiness; does not query devices.
 * No parent wires.
 */
uint32_t
gj_shell_ptr_kbd_caps_5439(void)
{
	(void)NULL;
	return b5439_shell_ptr_kbd_caps();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shell_ptr_kbd_caps_5439(void)
    __attribute__((alias("gj_shell_ptr_kbd_caps_5439")));
