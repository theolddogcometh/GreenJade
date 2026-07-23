/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9179: soft gamepad/hid id stub continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_gamepad_ready_u_9179(void);
 *     - Returns 1 (gamepad/hid soft id stub continuum ready). Pure-data
 *       product tag that exclusive wave 9171-9180 surfaces are present.
 *   uint32_t __gj_gamepad_ready_u_9179  (alias)
 *   __libcgj_batch9179_marker = "libcgj-batch9179"
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

const char __libcgj_batch9179_marker[] = "libcgj-batch9179";

#define B9179_GAMEPAD_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9179_gamepad_ready(void)
{
	return B9179_GAMEPAD_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gamepad_ready_u_9179 - gamepad/hid soft id stub continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive wave
 * surfaces are present. Does not call libinput/SDL or libc. No parent
 * wires.
 */
uint32_t
gj_gamepad_ready_u_9179(void)
{
	(void)NULL;
	return b9179_gamepad_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gamepad_ready_u_9179(void)
    __attribute__((alias("gj_gamepad_ready_u_9179")));
