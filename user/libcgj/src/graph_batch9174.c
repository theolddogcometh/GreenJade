/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9174: soft HID mouse usage id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_mouse_id_9174(void);
 *     - Returns 0x02 (HID Generic Desktop Mouse usage soft catalog
 *       value). Pure-data device-usage id stub; does not open mice or
 *       call libinput/SDL.
 *   uint32_t __gj_hid_mouse_id_9174  (alias)
 *   __libcgj_batch9174_marker = "libcgj-batch9174"
 *
 * Exclusive continuum CREATE-ONLY (9171-9180: gamepad/hid soft id
 * stubs — hid_usage_page_id_9171, hid_gamepad_id_9172,
 * hid_keyboard_id_9173, hid_mouse_id_9174, btn_a_id_9175,
 * btn_b_id_9176, btn_x_id_9177, btn_y_id_9178,
 * gamepad_ready_u_9179, batch_id_9180).
 * Unique surface only; no multi-def. Distinct from
 * gj_hid_keyboard_id_9173 (batch9173). No parent wires. No __int128.
 * No HID/gamepad implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9174_marker[] = "libcgj-batch9174";

/* Soft HID Mouse usage catalog id (Generic Desktop usage 0x02). */
#define B9174_HID_MOUSE  0x02u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9174_mouse_id(void)
{
	return B9174_HID_MOUSE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_mouse_id_9174 - soft HID Mouse usage catalog id.
 *
 * Always returns 0x02. Soft continuum constant; does not open mice or
 * call libinput/SDL. No parent wires.
 */
uint32_t
gj_hid_mouse_id_9174(void)
{
	(void)NULL;
	return b9174_mouse_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_mouse_id_9174(void)
    __attribute__((alias("gj_hid_mouse_id_9174")));
