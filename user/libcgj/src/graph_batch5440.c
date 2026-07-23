/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5440: batch identity for display-connect wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5440(void);
 *     - Returns the compile-time graph batch number for this TU (5440).
 *   uint32_t __gj_batch_id_5440  (alias)
 *   __libcgj_batch5440_marker = "libcgj-batch5440"
 *
 * Exclusive continuum CREATE-ONLY (5431-5440: X11/Wayland display connect,
 * wl_display, xcb connection stubs, input seat, pointer/keyboard
 * capabilities for product shell —
 * x11_display_connect_ok_5431, x11_display_proto_major_5432,
 * wl_display_connect_ok_5433, wl_display_fd_stub_5434,
 * xcb_connection_ok_5435, xcb_setup_roots_len_5436,
 * wl_seat_cap_pointer_5437, wl_seat_cap_keyboard_5438,
 * shell_ptr_kbd_caps_5439, batch_id_5440). Unique gj_batch_id_5440
 * surface only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5440_marker[] = "libcgj-batch5440";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5440_id(void)
{
	return 5440u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5440 - report this TU's graph batch number.
 *
 * Always returns 5440.
 */
uint32_t
gj_batch_id_5440(void)
{
	(void)NULL;
	return b5440_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5440(void)
    __attribute__((alias("gj_batch_id_5440")));
