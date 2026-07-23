/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5325: HID/Linux KEY_ENTER scancode (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_key_enter_u(void);
 *     - Returns the Linux EV_KEY KEY_ENTER code (28).
 *   uint32_t __gj_hid_key_enter_u  (alias)
 *   __libcgj_batch5325_marker = "libcgj-batch5325"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Unique gj_hid_key_enter_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5325_marker[] = "libcgj-batch5325";

/* Linux input-event-codes: KEY_ENTER. */
#define B5325_HID_KEY_ENTER  28u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5325_key_enter(void)
{
	return B5325_HID_KEY_ENTER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_key_enter_u - report the KEY_ENTER scancode.
 *
 * Always returns 28. Soft compile-time constant for freestanding HID/key
 * event packing; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_key_enter_u(void)
{
	(void)NULL;
	return b5325_key_enter();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_key_enter_u(void)
    __attribute__((alias("gj_hid_key_enter_u")));
