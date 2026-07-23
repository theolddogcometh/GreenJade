/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5951: Deck LCD panel horizontal active pixels.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_hactive_5951(void);
 *     - Returns Deck LCD stub HACTIVE (1280 native panel pixels).
 *   uint32_t __gj_deck_lcd_hactive_5951  (alias)
 *   __libcgj_batch5951_marker = "libcgj-batch5951"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_hactive_5951 surface only; no multi-def.
 * Distinct from gj_hactive_px_5741 and gj_deck_res_w_u. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5951_marker[] = "libcgj-batch5951";

/* Steam Deck LCD native horizontal active pixels. */
#define B5951_DECK_LCD_HACTIVE  1280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5951_deck_lcd_hactive(void)
{
	return B5951_DECK_LCD_HACTIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_hactive_5951 - report Deck LCD HACTIVE in pixels.
 *
 * Always returns 1280. Integer-only panel timing stub; does not program
 * a CRTC or eDP link. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_hactive_5951(void)
{
	(void)NULL;
	return b5951_deck_lcd_hactive();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_hactive_5951(void)
    __attribute__((alias("gj_deck_lcd_hactive_5951")));
