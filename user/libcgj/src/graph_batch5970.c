/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5970: batch identity for Deck controller USB wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5970(void);
 *     - Returns the compile-time graph batch number for this TU (5970).
 *   uint32_t gj_graph_milestone_5970(void);
 *     - Returns the current graph milestone revision (5970).
 *   uint32_t __gj_batch_id_5970  (alias)
 *   uint32_t __gj_graph_milestone_5970  (alias)
 *   __libcgj_batch5970_marker = "libcgj-batch5970"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs —
 * deck_usb_vid_5961, deck_ctrl_usb_pid_5962, deck_ctrl_pid_is_known_5963,
 * deck_serial_len_ok_5964, deck_serial_hex_digit_ok_5965,
 * deck_vid_is_valve_5966, deck_pid_is_controller_5967,
 * deck_usb_iface_ok_5968, deck_serial_nibble_pack_5969, batch_id_5970).
 * Does NOT redefine gj_batch_id / gj_graph_milestone. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5970_marker[] = "libcgj-batch5970";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5970_id(void)
{
	return 5970u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5970 - report this TU's graph batch number.
 *
 * Always returns 5970.
 */
uint32_t
gj_batch_id_5970(void)
{
	(void)NULL;
	return b5970_id();
}

/*
 * gj_graph_milestone_5970 - report this TU's graph milestone revision.
 *
 * Always returns 5970. Distinct from earlier gj_graph_milestone /
 * gj_graph_milestone_N so all can coexist at link time. No parent wires.
 */
uint32_t
gj_graph_milestone_5970(void)
{
	return b5970_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_5970(void)
    __attribute__((alias("gj_batch_id_5970")));

uint32_t __gj_graph_milestone_5970(void)
    __attribute__((alias("gj_graph_milestone_5970")));
