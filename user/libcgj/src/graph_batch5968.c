/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5968: Steam Deck controller USB interface soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_deck_usb_iface_ok_5968(uint32_t iface);
 *     - Return 1 if iface is a soft-valid USB bInterfaceNumber for a
 *       Deck / Steam controller composite device in [0, 7], else 0.
 *   int __gj_deck_usb_iface_ok_5968  (alias)
 *   __libcgj_batch5968_marker = "libcgj-batch5968"
 *
 * Exclusive continuum CREATE-ONLY (5961-5970: Steam Deck controller
 * serial / USB VID/PID stubs). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5968_marker[] = "libcgj-batch5968";

/* Inclusive soft max USB interface index for controller composites. */
#define B5968_IFACE_MAX  7u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5968_iface_ok(uint32_t uIface)
{
	if (uIface > B5968_IFACE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_usb_iface_ok_5968 - soft check for controller USB interface #.
 *
 * iface: bInterfaceNumber (zero-based)
 *
 * Returns 1 when iface <= 7, else 0. Does not enumerate devices.
 * No parent wires.
 */
int
gj_deck_usb_iface_ok_5968(uint32_t uIface)
{
	(void)NULL;
	return b5968_iface_ok(uIface);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_usb_iface_ok_5968(uint32_t uIface)
    __attribute__((alias("gj_deck_usb_iface_ok_5968")));
