/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5879: typical HID mouse RD length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_len_mouse_u(void);
 *     - Returns a soft typical mouse report descriptor length (52).
 *   uint32_t __gj_hid_rd_len_mouse_u  (alias)
 *   __libcgj_batch5879_marker = "libcgj-batch5879"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_len_mouse_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5879_marker[] = "libcgj-batch5879";

/* Soft typical 3-button mouse HID report descriptor length (bytes). */
#define B5879_HID_RD_LEN_MOUSE  52u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5879_rd_len_mouse(void)
{
	return B5879_HID_RD_LEN_MOUSE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_len_mouse_u - report a typical mouse HID RD length.
 *
 * Always returns 52. Soft compile-time size hint for freestanding HID
 * length checks; does not emit a descriptor. No parent wires.
 */
uint32_t
gj_hid_rd_len_mouse_u(void)
{
	(void)NULL;
	return b5879_rd_len_mouse();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_len_mouse_u(void)
    __attribute__((alias("gj_hid_rd_len_mouse_u")));
