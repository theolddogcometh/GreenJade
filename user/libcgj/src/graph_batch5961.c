/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5961: Steam Deck / Valve USB vendor-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_usb_vid_5961(void);
 *     - Returns the Valve Corporation USB vendor id used by Steam Deck
 *       and related controllers (0x28DE). Soft compile-time catalog stub.
 *   uint32_t __gj_deck_usb_vid_5961  (alias)
 *   __libcgj_batch5961_marker = "libcgj-batch5961"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs —
 * deck_usb_vid_5961, deck_ctrl_usb_pid_5962, deck_ctrl_pid_is_known_5963,
 * deck_serial_len_ok_5964, deck_serial_hex_digit_ok_5965,
 * deck_vid_is_valve_5966, deck_pid_is_controller_5967,
 * deck_usb_iface_ok_5968, deck_serial_nibble_pack_5969, batch_id_5970).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5961_marker[] = "libcgj-batch5961";

/* Valve Corporation USB vendor id (public idVendor). */
#define B5961_VALVE_USB_VID  0x28DEu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5961_deck_usb_vid(void)
{
	return B5961_VALVE_USB_VID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_usb_vid_5961 - report Valve USB vendor id for Deck controllers.
 *
 * Always returns 0x28DE. Soft catalog constant only; does not open USB.
 * No parent wires.
 */
uint32_t
gj_deck_usb_vid_5961(void)
{
	(void)NULL;
	return b5961_deck_usb_vid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_usb_vid_5961(void)
    __attribute__((alias("gj_deck_usb_vid_5961")));
