/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5627: Bluetooth LE advertising interval stub.
 *
 * Surface (unique symbols):
 *   int gj_bt_adv_int_ok_5627(uint32_t slots);
 *     - Return 1 if slots is in the closed range [32, 16384] (BLE adv
 *       interval in 0.625 ms units, matching common HCI bounds), else 0.
 *   int __gj_bt_adv_int_ok_5627  (alias)
 *   __libcgj_batch5627_marker = "libcgj-batch5627"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_bt_adv_int_ok_5627 surface
 * only; no multi-def. Distinct from gj_bt_chan_ok_5624. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5627_marker[] = "libcgj-batch5627";

/* Soft BLE advertising interval bounds (0.625 ms units). */
#define B5627_ADV_MIN  32u
#define B5627_ADV_MAX  16384u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5627_adv_int_ok(uint32_t uSlots)
{
	if (uSlots < B5627_ADV_MIN) {
		return 0;
	}
	if (uSlots > B5627_ADV_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bt_adv_int_ok_5627 - soft check BLE advertising interval slots.
 *
 * slots: candidate adv interval in 0.625 ms units.
 * Returns 1 when slots is in [32, 16384], else 0.
 * Does not call libc or talk to HCI. No parent wires.
 */
int
gj_bt_adv_int_ok_5627(uint32_t uSlots)
{
	(void)NULL;
	return b5627_adv_int_ok(uSlots);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bt_adv_int_ok_5627(uint32_t uSlots)
    __attribute__((alias("gj_bt_adv_int_ok_5627")));
