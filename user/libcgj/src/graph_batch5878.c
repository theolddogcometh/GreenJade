/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5878: typical HID keyboard RD length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_rd_len_keyboard_u(void);
 *     - Returns a soft typical keyboard report descriptor length (63).
 *   uint32_t __gj_hid_rd_len_keyboard_u  (alias)
 *   __libcgj_batch5878_marker = "libcgj-batch5878"
 *
 * Exclusive continuum CREATE-ONLY (5871-5880: HID report descriptor
 * length/checksum stubs). Unique gj_hid_rd_len_keyboard_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5878_marker[] = "libcgj-batch5878";

/* Soft typical full-keyboard HID report descriptor length (bytes). */
#define B5878_HID_RD_LEN_KEYBOARD  63u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5878_rd_len_keyboard(void)
{
	return B5878_HID_RD_LEN_KEYBOARD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_rd_len_keyboard_u - report a typical keyboard HID RD length.
 *
 * Always returns 63. Soft compile-time size hint for freestanding HID
 * length checks; does not emit a descriptor. No parent wires.
 */
uint32_t
gj_hid_rd_len_keyboard_u(void)
{
	(void)NULL;
	return b5878_rd_len_keyboard();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_rd_len_keyboard_u(void)
    __attribute__((alias("gj_hid_rd_len_keyboard_u")));
