/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5966: Valve USB vendor-id match soft predicate.
 *
 * Surface (unique symbols):
 *   int gj_deck_vid_is_valve_5966(uint32_t vid);
 *     - Return 1 if vid equals the Valve Corporation USB vendor id
 *       0x28DE (Steam Deck / Steam controller family), else 0.
 *   int __gj_deck_vid_is_valve_5966  (alias)
 *   __libcgj_batch5966_marker = "libcgj-batch5966"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * Distinct from gj_deck_usb_vid_5961 (constant getter). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5966_marker[] = "libcgj-batch5966";

/* Valve Corporation USB vendor id (public idVendor). */
#define B5966_VALVE_USB_VID  0x28DEu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5966_vid_is_valve(uint32_t uVid)
{
	if (uVid == B5966_VALVE_USB_VID) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_vid_is_valve_5966 - soft check that vid is Valve (0x28DE).
 *
 * vid: USB vendor id (16-bit value zero-extended to uint32_t)
 *
 * Returns 1 iff vid == 0x28DE, else 0. No parent wires.
 */
int
gj_deck_vid_is_valve_5966(uint32_t uVid)
{
	(void)NULL;
	return b5966_vid_is_valve(uVid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_vid_is_valve_5966(uint32_t uVid)
    __attribute__((alias("gj_deck_vid_is_valve_5966")));
