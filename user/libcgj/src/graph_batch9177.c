/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9177: soft gamepad button X catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_btn_x_id_9177(void);
 *     - Returns 0x133 (Linux input BTN_X / BTN_NORTH soft catalog
 *       value). Pure-data button id stub; does not poll gamepads or
 *       call libinput/SDL.
 *   uint32_t __gj_btn_x_id_9177  (alias)
 *   __libcgj_batch9177_marker = "libcgj-batch9177"
 *
 * Exclusive continuum CREATE-ONLY (9171-9180: gamepad/hid soft id
 * stubs — hid_usage_page_id_9171, hid_gamepad_id_9172,
 * hid_keyboard_id_9173, hid_mouse_id_9174, btn_a_id_9175,
 * btn_b_id_9176, btn_x_id_9177, btn_y_id_9178,
 * gamepad_ready_u_9179, batch_id_9180).
 * Unique surface only; no multi-def. Distinct from
 * gj_btn_b_id_9176 (batch9176). No parent wires. No __int128.
 * No HID/gamepad implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9177_marker[] = "libcgj-batch9177";

/* Soft gamepad Button X catalog id (Linux BTN_X = 0x133). */
#define B9177_BTN_X  0x133u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9177_btn_x_id(void)
{
	return B9177_BTN_X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btn_x_id_9177 - soft gamepad Button X catalog id.
 *
 * Always returns 0x133. Soft continuum constant; does not poll
 * gamepads or call libinput/SDL. No parent wires.
 */
uint32_t
gj_btn_x_id_9177(void)
{
	(void)NULL;
	return b9177_btn_x_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_btn_x_id_9177(void)
    __attribute__((alias("gj_btn_x_id_9177")));
