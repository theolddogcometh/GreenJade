/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5959: Deck LCD panel horizontal front porch.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_hfp_5959(void);
 *     - Returns Deck LCD stub HFP in pixels (48).
 *   uint32_t __gj_deck_lcd_hfp_5959  (alias)
 *   __libcgj_batch5959_marker = "libcgj-batch5959"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_hfp_5959 surface only; no multi-def.
 * Completes the soft blanking model with hactive/htotal/hsync. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5959_marker[] = "libcgj-batch5959";

/* Stub HFP for Deck LCD reduced-blanking soft table. */
#define B5959_DECK_LCD_HFP  48u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5959_deck_lcd_hfp(void)
{
	return B5959_DECK_LCD_HFP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_hfp_5959 - report Deck LCD horizontal front porch (pixels).
 *
 * Always returns 48. Integer-only panel timing stub; does not program
 * a CRTC. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_hfp_5959(void)
{
	(void)NULL;
	return b5959_deck_lcd_hfp();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_hfp_5959(void)
    __attribute__((alias("gj_deck_lcd_hfp_5959")));
