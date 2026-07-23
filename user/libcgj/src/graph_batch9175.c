/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9175: soft gamepad button A catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_btn_a_id_9175(void);
 *     - Returns 0x130 (Linux input BTN_A / BTN_SOUTH soft catalog
 *       value). Pure-data button id stub; does not poll gamepads or
 *       call libinput/SDL.
 *   uint32_t __gj_btn_a_id_9175  (alias)
 *   __libcgj_batch9175_marker = "libcgj-batch9175"
 *
 * Exclusive continuum CREATE-ONLY (9171-9180: gamepad/hid soft id
 * stubs — hid_usage_page_id_9171, hid_gamepad_id_9172,
 * hid_keyboard_id_9173, hid_mouse_id_9174, btn_a_id_9175,
 * btn_b_id_9176, btn_x_id_9177, btn_y_id_9178,
 * gamepad_ready_u_9179, batch_id_9180).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No HID/gamepad implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9175_marker[] = "libcgj-batch9175";

/* Soft gamepad Button A catalog id (Linux BTN_A = 0x130). */
#define B9175_BTN_A  0x130u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9175_btn_a_id(void)
{
	return B9175_BTN_A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_btn_a_id_9175 - soft gamepad Button A catalog id.
 *
 * Always returns 0x130. Soft continuum constant; does not poll
 * gamepads or call libinput/SDL. No parent wires.
 */
uint32_t
gj_btn_a_id_9175(void)
{
	(void)NULL;
	return b9175_btn_a_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_btn_a_id_9175(void)
    __attribute__((alias("gj_btn_a_id_9175")));
