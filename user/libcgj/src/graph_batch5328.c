/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5328: HID Shift modifier bit mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_mod_shift_u(void);
 *     - Returns the HID keyboard Shift modifier bit mask (1 << 1 == 2).
 *   uint32_t __gj_hid_mod_shift_u  (alias)
 *   __libcgj_batch5328_marker = "libcgj-batch5328"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Unique gj_hid_mod_shift_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5328_marker[] = "libcgj-batch5328";

/* HID keyboard modifier bit: Shift = bit 1. */
#define B5328_HID_MOD_SHIFT  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5328_mod_shift(void)
{
	return B5328_HID_MOD_SHIFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_mod_shift_u - report the HID Shift modifier bit mask.
 *
 * Always returns 2 (1 << 1). Soft compile-time constant for freestanding
 * HID modifier packing; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_mod_shift_u(void)
{
	(void)NULL;
	return b5328_mod_shift();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_mod_shift_u(void)
    __attribute__((alias("gj_hid_mod_shift_u")));
