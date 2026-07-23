/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5455: Steam Deck HDR capability stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_hdr_capable_5455(void);
 *     - Returns 0 (HDR capability stub = not claimed; LCD baseline).
 *       Soft compile-time display-mode product stub; not a live
 *       connector HDR metadata probe.
 *   uint32_t __gj_deck_hdr_capable_5455  (alias)
 *   __libcgj_batch5455_marker = "libcgj-batch5455"
 *
 * Exclusive continuum CREATE-ONLY (5451-5460: Deck power/thermal and
 * display stubs unique). Unique gj_deck_hdr_capable_5455 surface only;
 * no multi-def. Distinct from gj_deck_refresh_hz_5454 (batch5454).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5455_marker[] = "libcgj-batch5455";

/* HDR capability stub (0 = not claimed / LCD baseline). */
#define B5455_HDR_CAPABLE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5455_hdr_capable(void)
{
	return B5455_HDR_CAPABLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_hdr_capable_5455 - report Steam Deck HDR capability stub.
 *
 * Always returns 0 (capability not claimed). Soft pure-data display
 * stub; does not read EDID/HDR metadata. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_deck_hdr_capable_5455(void)
{
	(void)NULL;
	return b5455_hdr_capable();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_hdr_capable_5455(void)
    __attribute__((alias("gj_deck_hdr_capable_5455")));
