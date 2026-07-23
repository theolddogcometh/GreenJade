/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5626: WiFi band validity stub.
 *
 * Surface (unique symbols):
 *   int gj_wifi_band_ok_5626(uint32_t band);
 *     - Return 1 if band is in the closed range [0, 2] soft band tags
 *       (0 = 2.4 GHz, 1 = 5 GHz, 2 = 6 GHz), else 0.
 *   int __gj_wifi_band_ok_5626  (alias)
 *   __libcgj_batch5626_marker = "libcgj-batch5626"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_wifi_band_ok_5626 surface
 * only; no multi-def. Distinct from gj_wifi_chan_ok_5623. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5626_marker[] = "libcgj-batch5626";

/* Soft WiFi band tags: 0=2.4, 1=5, 2=6 GHz. */
#define B5626_BAND_MAX  2u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5626_band_ok(uint32_t uBand)
{
	if (uBand > B5626_BAND_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wifi_band_ok_5626 - soft check that a WiFi band tag is known.
 *
 * band: 0 = 2.4 GHz, 1 = 5 GHz, 2 = 6 GHz.
 * Returns 1 when band is in [0, 2], else 0.
 * Does not call libc. No parent wires.
 */
int
gj_wifi_band_ok_5626(uint32_t uBand)
{
	(void)NULL;
	return b5626_band_ok(uBand);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wifi_band_ok_5626(uint32_t uBand)
    __attribute__((alias("gj_wifi_band_ok_5626")));
