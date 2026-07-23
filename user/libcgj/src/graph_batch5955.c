/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5955: Deck LCD panel pixel clock (kHz).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_lcd_pixel_clock_khz_5955(void);
 *     - Returns Deck LCD stub pixel clock in kHz (71107 ≈ 1440*823*60
 *       / 1000 for 1280x800@60 reduced blanking).
 *   uint32_t __gj_deck_lcd_pixel_clock_khz_5955  (alias)
 *   __libcgj_batch5955_marker = "libcgj-batch5955"
 *
 * Exclusive continuum CREATE-ONLY (5951-5960: Deck LCD panel timing
 * stubs). Unique gj_deck_lcd_pixel_clock_khz_5955 surface only; no
 * multi-def. Distinct from gj_pixel_clock_khz_5745. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5955_marker[] = "libcgj-batch5955";

/*
 * Stub pixel clock: floor(1440 * 823 * 60 / 1000) = 71107 kHz
 * (~71.1 MHz Deck LCD soft model).
 */
#define B5955_DECK_LCD_PIXCLK_KHZ  71107u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5955_deck_lcd_pixel_clock_khz(void)
{
	return B5955_DECK_LCD_PIXCLK_KHZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_lcd_pixel_clock_khz_5955 - report Deck LCD pixel clock (kHz).
 *
 * Always returns 71107. Integer-only panel timing stub; does not
 * program a PLL. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_lcd_pixel_clock_khz_5955(void)
{
	(void)NULL;
	return b5955_deck_lcd_pixel_clock_khz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_lcd_pixel_clock_khz_5955(void)
    __attribute__((alias("gj_deck_lcd_pixel_clock_khz_5955")));
