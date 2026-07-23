/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5958: Deck LCD panel VSYNC pulse width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_vsync_5958(void);
 *     - Returns Deck LCD stub VSYNC width in lines (6).
 *   uint32_t __gj_deck_lcd_vsync_5958  (alias)
 *   __libcgj_batch5958_marker = "libcgj-batch5958"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_vsync_5958 surface only; no multi-def.
 * Distinct from gj_vsync_width_5748. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5958_marker[] = "libcgj-batch5958";

/* Stub VSYNC pulse width for Deck LCD reduced-blanking soft table. */
#define B5958_DECK_LCD_VSYNC  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5958_deck_lcd_vsync(void)
{
	return B5958_DECK_LCD_VSYNC;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_vsync_5958 - report Deck LCD VSYNC pulse width (lines).
 *
 * Always returns 6. Integer-only panel timing stub; does not program
 * a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_vsync_5958(void)
{
	(void)NULL;
	return b5958_deck_lcd_vsync();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_vsync_5958(void)
    __attribute__((alias("gj_deck_lcd_vsync_5958")));
