/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9171: soft HID usage-page catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_hid_usage_page_id_9171(void);
 *     - Returns 0x01 (HID Generic Desktop usage page soft catalog
 *       value). Pure-data usage-page id stub; does not open HID
 *       devices or call libinput/SDL.
 *   uint32_t __gj_hid_usage_page_id_9171  (alias)
 *   __libcgj_batch9171_marker = "libcgj-batch9171"
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

const char __libcgj_batch9171_marker[] = "libcgj-batch9171";

/* Soft HID Generic Desktop usage page catalog id (0x01). */
#define B9171_HID_USAGE_PAGE  0x01u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9171_usage_page_id(void)
{
	return B9171_HID_USAGE_PAGE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hid_usage_page_id_9171 - soft HID Generic Desktop usage-page id.
 *
 * Always returns 0x01. Soft continuum constant; does not open HID
 * devices or call libinput/SDL. No parent wires.
 */
uint32_t
gj_hid_usage_page_id_9171(void)
{
	(void)NULL;
	return b9171_usage_page_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hid_usage_page_id_9171(void)
    __attribute__((alias("gj_hid_usage_page_id_9171")));
