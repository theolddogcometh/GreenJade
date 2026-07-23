/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5322: HID right mouse button bit mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_btn_right_u(void);
 *     - Returns the HID/pointer right-button bit mask (1 << 1 == 2).
 *   uint32_t __gj_hid_btn_right_u  (alias)
 *   __libcgj_batch5322_marker = "libcgj-batch5322"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Unique gj_hid_btn_right_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5322_marker[] = "libcgj-batch5322";

/* HID pointer button bit: right = bit 1. */
#define B5322_HID_BTN_RIGHT  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5322_btn_right(void)
{
	return B5322_HID_BTN_RIGHT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_btn_right_u - report the HID right mouse button bit mask.
 *
 * Always returns 2 (1 << 1). Soft compile-time constant for freestanding
 * HID/pointer button packing; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_btn_right_u(void)
{
	(void)NULL;
	return b5322_btn_right();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_btn_right_u(void)
    __attribute__((alias("gj_hid_btn_right_u")));
