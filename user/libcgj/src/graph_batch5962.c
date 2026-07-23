/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5962: Steam Deck controller USB product-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_ctrl_usb_pid_5962(void);
 *     - Returns the primary Steam Deck (Jupiter) onboard controller USB
 *       product id (0x1205). Soft compile-time catalog stub.
 *   uint32_t __gj_deck_ctrl_usb_pid_5962  (alias)
 *   __libcgj_batch5962_marker = "libcgj-batch5962"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * Distinct from gj_deck_usb_vid_5961. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5962_marker[] = "libcgj-batch5962";

/* Steam Deck (Jupiter) controller USB product id (public idProduct). */
#define B5962_DECK_CTRL_USB_PID  0x1205u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5962_deck_ctrl_usb_pid(void)
{
	return B5962_DECK_CTRL_USB_PID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_ctrl_usb_pid_5962 - report Deck controller USB product id.
 *
 * Always returns 0x1205. Soft catalog constant only; does not open USB.
 * No parent wires.
 */
uint32_t
gj_deck_ctrl_usb_pid_5962(void)
{
	(void)NULL;
	return b5962_deck_ctrl_usb_pid();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_ctrl_usb_pid_5962(void)
    __attribute__((alias("gj_deck_ctrl_usb_pid_5962")));
