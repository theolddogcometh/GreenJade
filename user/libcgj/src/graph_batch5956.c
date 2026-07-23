/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5956: Deck LCD panel refresh rate (Hz).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_refresh_hz_5956(void);
 *     - Returns Deck LCD stub vertical refresh in Hz (60).
 *   uint32_t __gj_deck_lcd_refresh_hz_5956  (alias)
 *   __libcgj_batch5956_marker = "libcgj-batch5956"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_refresh_hz_5956 surface only; no multi-def.
 * Distinct from gj_refresh_hz_5746. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5956_marker[] = "libcgj-batch5956";

/* Steam Deck LCD common native vertical refresh. */
#define B5956_DECK_LCD_REFRESH_HZ  60u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5956_deck_lcd_refresh_hz(void)
{
	return B5956_DECK_LCD_REFRESH_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_refresh_hz_5956 - report Deck LCD vertical refresh (Hz).
 *
 * Always returns 60. Integer-only panel timing stub; does not program
 * a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_refresh_hz_5956(void)
{
	(void)NULL;
	return b5956_deck_lcd_refresh_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_refresh_hz_5956(void)
    __attribute__((alias("gj_deck_lcd_refresh_hz_5956")));
