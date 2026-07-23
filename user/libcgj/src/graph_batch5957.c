/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5957: Deck LCD panel HSYNC pulse width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_hsync_5957(void);
 *     - Returns Deck LCD stub HSYNC width in pixels (32).
 *   uint32_t __gj_deck_lcd_hsync_5957  (alias)
 *   __libcgj_batch5957_marker = "libcgj-batch5957"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_hsync_5957 surface only; no multi-def.
 * Distinct from gj_hsync_width_5747. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5957_marker[] = "libcgj-batch5957";

/* Stub HSYNC pulse width for Deck LCD reduced-blanking soft table. */
#define B5957_DECK_LCD_HSYNC  32u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5957_deck_lcd_hsync(void)
{
	return B5957_DECK_LCD_HSYNC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_hsync_5957 - report Deck LCD HSYNC pulse width (pixels).
 *
 * Always returns 32. Integer-only panel timing stub; does not program
 * a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_hsync_5957(void)
{
	(void)NULL;
	return b5957_deck_lcd_hsync();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_hsync_5957(void)
    __attribute__((alias("gj_deck_lcd_hsync_5957")));
