/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5621: WiFi radio ready flag stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wifi_radio_ready_5621(void);
 *     - Returns 1 (soft compile-time WiFi radio ready lamp for Deck
 *       handheld / desktop product continuum). Stub integer only.
 *   uint32_t __gj_wifi_radio_ready_5621  (alias)
 *   __libcgj_batch5621_marker = "libcgj-batch5621"
 *
 * Bluetooth / WiFi radio stubs + Deck handheld radio-ready exclusive
 * CREATE-ONLY wave (5621-5630). Unique gj_wifi_radio_ready_5621 surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5621_marker[] = "libcgj-batch5621";

/* Soft WiFi radio ready lamp. */
#define B5621_WIFI_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5621_wifi_ready(void)
{
	return B5621_WIFI_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wifi_radio_ready_5621 - report WiFi radio ready flag stub.
 *
 * Always returns 1 (WiFi radio ready). Link-time presence tags the
 * radio continuum. Does not probe hardware or call libc. No parent wires.
 */
uint32_t
gj_wifi_radio_ready_5621(void)
{
	(void)NULL;
	return b5621_wifi_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wifi_radio_ready_5621(void)
    __attribute__((alias("gj_wifi_radio_ready_5621")));
