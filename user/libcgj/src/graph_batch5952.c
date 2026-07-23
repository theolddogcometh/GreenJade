/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5952: Deck LCD panel vertical active lines.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_vactive_5952(void);
 *     - Returns Deck LCD stub VACTIVE (800 native panel lines).
 *   uint32_t __gj_deck_lcd_vactive_5952  (alias)
 *   __libcgj_batch5952_marker = "libcgj-batch5952"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_vactive_5952 surface only; no multi-def.
 * Distinct from gj_vactive_px_5742 and gj_deck_res_h_u. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5952_marker[] = "libcgj-batch5952";

/* Steam Deck LCD native vertical active lines. */
#define B5952_DECK_LCD_VACTIVE  800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5952_deck_lcd_vactive(void)
{
	return B5952_DECK_LCD_VACTIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_vactive_5952 - report Deck LCD VACTIVE in lines.
 *
 * Always returns 800. Integer-only panel timing stub; does not program
 * a CRTC or eDP link. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_vactive_5952(void)
{
	(void)NULL;
	return b5952_deck_lcd_vactive();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_vactive_5952(void)
    __attribute__((alias("gj_deck_lcd_vactive_5952")));
