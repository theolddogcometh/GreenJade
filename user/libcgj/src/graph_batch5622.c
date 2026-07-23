/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5622: Bluetooth radio ready flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bt_radio_ready_5622(void);
 *     - Returns 1 (soft compile-time Bluetooth radio ready lamp for
 *       Deck handheld / desktop product continuum). Stub integer only.
 *   uint32_t __gj_bt_radio_ready_5622  (alias)
 *   __libcgj_batch5622_marker = "libcgj-batch5622"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_bt_radio_ready_5622 surface
 * only; no multi-def. Distinct from gj_wifi_radio_ready_5621. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5622_marker[] = "libcgj-batch5622";

/* Soft Bluetooth radio ready lamp. */
#define B5622_BT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5622_bt_ready(void)
{
	return B5622_BT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bt_radio_ready_5622 - report Bluetooth radio ready flag stub.
 *
 * Always returns 1 (Bluetooth radio ready). Does not open HCI sockets
 * or call libc. No parent wires.
 */
uint32_t
gj_bt_radio_ready_5622(void)
{
	(void)NULL;
	return b5622_bt_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bt_radio_ready_5622(void)
    __attribute__((alias("gj_bt_radio_ready_5622")));
