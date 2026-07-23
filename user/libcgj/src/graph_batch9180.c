/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9180: gamepad/hid soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9180(void);
 *     - Returns the compile-time graph batch number for this TU (9180).
 *   uint32_t __gj_batch_id_9180  (alias)
 *   __libcgj_batch9180_marker = "libcgj-batch9180"
 *
 * Exclusive continuum CREATE-ONLY (9171-9180: gamepad/hid soft id
 * stubs — hid_usage_page_id_9171, hid_gamepad_id_9172,
 * hid_keyboard_id_9173, hid_mouse_id_9174, btn_a_id_9175,
 * btn_b_id_9176, btn_x_id_9177, btn_y_id_9178,
 * gamepad_ready_u_9179, batch_id_9180).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No HID/gamepad implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9180_marker[] = "libcgj-batch9180";

#define B9180_BATCH_ID  9180u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9180_id(void)
{
	return B9180_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9180 - report this TU's graph batch number.
 *
 * Always returns 9180. Link-time presence tags the exclusive
 * gamepad/hid soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9180(void)
{
	(void)NULL;
	return b9180_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9180(void)
    __attribute__((alias("gj_batch_id_9180")));
