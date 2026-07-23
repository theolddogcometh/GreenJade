/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5321: HID left mouse button bit mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_btn_left_u(void);
 *     - Returns the HID/pointer left-button bit mask (1 << 0 == 1).
 *   uint32_t __gj_hid_btn_left_u  (alias)
 *   __libcgj_batch5321_marker = "libcgj-batch5321"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Unique gj_hid_btn_left_u surface only;
 * no multi-def. Distinct from gj_mouse_btn_left_p (batch1911). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5321_marker[] = "libcgj-batch5321";

/* HID pointer button bit: left = bit 0. */
#define B5321_HID_BTN_LEFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5321_btn_left(void)
{
	return B5321_HID_BTN_LEFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_btn_left_u - report the HID left mouse button bit mask.
 *
 * Always returns 1 (1 << 0). Soft compile-time constant for freestanding
 * HID/pointer button packing; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_btn_left_u(void)
{
	(void)NULL;
	return b5321_btn_left();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_btn_left_u(void)
    __attribute__((alias("gj_hid_btn_left_u")));
