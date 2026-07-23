/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5624: Bluetooth classic channel validity stub.
 *
 * Surface (unique symbols):
 *   int gj_bt_chan_ok_5624(uint32_t chan);
 *     - Return 1 if chan is in the closed range [0, 78] (Bluetooth
 *       Classic 79 AFH hop channels), else 0. Soft pure-data gate only.
 *   int __gj_bt_chan_ok_5624  (alias)
 *   __libcgj_batch5624_marker = "libcgj-batch5624"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_bt_chan_ok_5624 surface only;
 * no multi-def. Distinct from gj_wifi_chan_ok_5623. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5624_marker[] = "libcgj-batch5624";

/* Bluetooth Classic hop channels 0..78 inclusive. */
#define B5624_BT_CHAN_MAX  78u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5624_bt_chan_ok(uint32_t uChan)
{
	if (uChan > B5624_BT_CHAN_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bt_chan_ok_5624 - soft check that a BT Classic hop channel is valid.
 *
 * chan: candidate AFH hop index (0..78).
 * Returns 1 when chan is in [0, 78], else 0.
 * Does not call libc or talk to HCI. No parent wires.
 */
int
gj_bt_chan_ok_5624(uint32_t uChan)
{
	(void)NULL;
	return b5624_bt_chan_ok(uChan);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bt_chan_ok_5624(uint32_t uChan)
    __attribute__((alias("gj_bt_chan_ok_5624")));
