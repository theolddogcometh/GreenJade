/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5329: HID Alt modifier bit mask (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_mod_alt_u(void);
 *     - Returns the HID keyboard Alt modifier bit mask (1 << 2 == 4).
 *   uint32_t __gj_hid_mod_alt_u  (alias)
 *   __libcgj_batch5329_marker = "libcgj-batch5329"
 *
 * Exclusive continuum CREATE-ONLY (5321-5330: input HID unique —
 * hid_btn_left_u, hid_btn_right_u, hid_btn_middle_u, hid_key_esc_u,
 * hid_key_enter_u, hid_key_space_u, hid_mod_ctrl_u, hid_mod_shift_u,
 * hid_mod_alt_u, batch_id_5330). Unique gj_hid_mod_alt_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5329_marker[] = "libcgj-batch5329";

/* HID keyboard modifier bit: Alt = bit 2. */
#define B5329_HID_MOD_ALT  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5329_mod_alt(void)
{
	return B5329_HID_MOD_ALT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_mod_alt_u - report the HID Alt modifier bit mask.
 *
 * Always returns 4 (1 << 2). Soft compile-time constant for freestanding
 * HID modifier packing; does not probe hardware. No parent wires.
 */
uint32_t
gj_hid_mod_alt_u(void)
{
	(void)NULL;
	return b5329_mod_alt();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_mod_alt_u(void)
    __attribute__((alias("gj_hid_mod_alt_u")));
