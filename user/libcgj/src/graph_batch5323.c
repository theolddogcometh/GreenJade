/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5323: HID middle mouse button bit mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_btn_middle_u(void);
 *     - Returns the HID/pointer middle-button bit mask (1 << 2 == 4).
 *   uint32_t __gj_hid_btn_middle_u  (alias)
 *   __libcgj_batch5323_marker = "libcgj-batch5323"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Unique gj_hid_btn_middle_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5323_marker[] = "libcgj-batch5323";

/* HID pointer button bit: middle = bit 2. */
#define B5323_HID_BTN_MIDDLE  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5323_btn_middle(void)
{
	return B5323_HID_BTN_MIDDLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_btn_middle_u - report the HID middle mouse button bit mask.
 *
 * Always returns 4 (1 << 2). Soft compile-time constant for freestanding
 * HID/pointer button packing; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_btn_middle_u(void)
{
	(void)NULL;
	return b5323_btn_middle();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_btn_middle_u(void)
    __attribute__((alias("gj_hid_btn_middle_u")));
