/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5628: WiFi RSSI soft-validity stub.
 *
 * Surface (unique symbols):
 *   int gj_wifi_rssi_ok_5628(int rssi_dbm);
 *     - Return 1 if rssi_dbm is in the closed soft range [-100, 0]
 *       (typical 802.11 RSSI dBm product hints), else 0.
 *   int __gj_wifi_rssi_ok_5628  (alias)
 *   __libcgj_batch5628_marker = "libcgj-batch5628"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_wifi_rssi_ok_5628 surface
 * only; no multi-def. Distinct from channel/band stubs. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5628_marker[] = "libcgj-batch5628";

/* Soft RSSI dBm product range. */
#define B5628_RSSI_MIN  (-100)
#define B5628_RSSI_MAX  0

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5628_rssi_ok(int nRssi)
{
	if (nRssi < B5628_RSSI_MIN) {
		return 0;
	}
	if (nRssi > B5628_RSSI_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wifi_rssi_ok_5628 - soft check that an RSSI dBm value is plausible.
 *
 * rssi_dbm: candidate received-signal strength in dBm (signed).
 * Returns 1 when rssi_dbm is in [-100, 0], else 0.
 * Does not call libc or read nl80211. No parent wires.
 */
int
gj_wifi_rssi_ok_5628(int nRssi)
{
	(void)NULL;
	return b5628_rssi_ok(nRssi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wifi_rssi_ok_5628(int nRssi)
    __attribute__((alias("gj_wifi_rssi_ok_5628")));
