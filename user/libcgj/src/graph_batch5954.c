/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5954: Deck LCD panel vertical total lines.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_vtotal_5954(void);
 *     - Returns Deck LCD stub VTOTAL (823 lines, reduced-blanking soft
 *       model for 1280x800@60).
 *   uint32_t __gj_deck_lcd_vtotal_5954  (alias)
 *   __libcgj_batch5954_marker = "libcgj-batch5954"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_vtotal_5954 surface only; no multi-def.
 * Distinct from gj_vtotal_5744. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5954_marker[] = "libcgj-batch5954";

/*
 * Stub VTOTAL: 800 active + 3 VFP + 6 VSYNC + 14 VBP = 823
 * (Deck LCD reduced-blanking soft table).
 */
#define B5954_DECK_LCD_VTOTAL  823u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5954_deck_lcd_vtotal(void)
{
	return B5954_DECK_LCD_VTOTAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_vtotal_5954 - report Deck LCD VTOTAL in lines.
 *
 * Always returns 823. Integer-only panel timing stub; does not program
 * a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_vtotal_5954(void)
{
	(void)NULL;
	return b5954_deck_lcd_vtotal();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_vtotal_5954(void)
    __attribute__((alias("gj_deck_lcd_vtotal_5954")));
