/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5953: Deck LCD panel horizontal total clocks.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_htotal_5953(void);
 *     - Returns Deck LCD stub HTOTAL (1440 clocks, reduced-blanking
 *       soft model for 1280x800@60).
 *   uint32_t __gj_deck_lcd_htotal_5953  (alias)
 *   __libcgj_batch5953_marker = "libcgj-batch5953"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_htotal_5953 surface only; no multi-def.
 * Distinct from gj_htotal_5743. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5953_marker[] = "libcgj-batch5953";

/*
 * Stub HTOTAL: 1280 active + 48 HFP + 32 HSYNC + 80 HBP = 1440
 * (Deck LCD reduced-blanking soft table).
 */
#define B5953_DECK_LCD_HTOTAL  1440u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5953_deck_lcd_htotal(void)
{
	return B5953_DECK_LCD_HTOTAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_htotal_5953 - report Deck LCD HTOTAL in pixel clocks.
 *
 * Always returns 1440. Integer-only panel timing stub; does not program
 * a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_htotal_5953(void)
{
	(void)NULL;
	return b5953_deck_lcd_htotal();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_htotal_5953(void)
    __attribute__((alias("gj_deck_lcd_htotal_5953")));
