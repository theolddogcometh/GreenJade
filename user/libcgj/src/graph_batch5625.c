/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5625: Deck handheld dual-radio ready flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_radio_ready_5625(void);
 *     - Returns 1 (soft compile-time Deck handheld radio-ready lamp:
 *       product path assumes WiFi + Bluetooth radio stubs are present).
 *   uint32_t __gj_deck_radio_ready_5625  (alias)
 *   __libcgj_batch5625_marker = "libcgj-batch5625"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_deck_radio_ready_5625 surface
 * only; no multi-def. Distinct from wifi/bt ready lamps. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5625_marker[] = "libcgj-batch5625";

/* Soft Deck handheld dual-radio ready lamp. */
#define B5625_DECK_RADIO_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5625_deck_radio_ready(void)
{
	return B5625_DECK_RADIO_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_radio_ready_5625 - report Deck handheld radio-ready flag stub.
 *
 * Always returns 1. Does not probe rfkill, DMI, or call libc.
 * No parent wires.
 */
uint32_t
gj_deck_radio_ready_5625(void)
{
	(void)NULL;
	return b5625_deck_radio_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_radio_ready_5625(void)
    __attribute__((alias("gj_deck_radio_ready_5625")));
