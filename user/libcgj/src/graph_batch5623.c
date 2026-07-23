/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5623: WiFi channel validity stub.
 *
 * Surface (unique symbols):
 *   int gj_wifi_chan_ok_5623(uint32_t chan);
 *     - Return 1 if chan is in the closed soft range [1, 165] (common
 *       2.4/5/6 GHz channel numbers used as product hints), else 0.
 *   int __gj_wifi_chan_ok_5623  (alias)
 *   __libcgj_batch5623_marker = "libcgj-batch5623"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_wifi_chan_ok_5623 surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5623_marker[] = "libcgj-batch5623";

/* Soft WiFi channel range (product hint, not regulatory). */
#define B5623_CHAN_MIN  1u
#define B5623_CHAN_MAX  165u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5623_chan_ok(uint32_t uChan)
{
	if (uChan < B5623_CHAN_MIN) {
		return 0;
	}
	if (uChan > B5623_CHAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wifi_chan_ok_5623 - soft check that a WiFi channel number is plausible.
 *
 * chan: candidate 802.11 channel number.
 * Returns 1 when chan is in [1, 165], else 0.
 * Does not call libc or consult regulatory databases. No parent wires.
 */
int
gj_wifi_chan_ok_5623(uint32_t uChan)
{
	(void)NULL;
	return b5623_chan_ok(uChan);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wifi_chan_ok_5623(uint32_t uChan)
    __attribute__((alias("gj_wifi_chan_ok_5623")));
