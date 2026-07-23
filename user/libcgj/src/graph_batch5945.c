/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5945: Deck OLED horizontal blanking pixels.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_oled_h_blank_5945(void);
 *     - Returns 64 (soft stub total horizontal blanking in pixels:
 *       front porch + sync + back porch).
 *   uint32_t __gj_deck_oled_h_blank_5945  (alias)
 *   __libcgj_batch5945_marker = "libcgj-batch5945"
 *
 * Exclusive continuum CREATE-ONLY (5941-5950: Deck OLED panel timing
 * stubs —
 * deck_oled_refresh_max_hz_5941, deck_oled_refresh_min_hz_5942,
 * deck_oled_h_active_5943, deck_oled_v_active_5944,
 * deck_oled_h_blank_5945, deck_oled_v_blank_5946,
 * deck_oled_frame_us_90_5947, deck_oled_pixel_clk_khz_5948,
 * deck_oled_timing_ready_5949, batch_id_5950). Unique surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5945_marker[] = "libcgj-batch5945";

/* Soft stub: total horizontal blanking (pixels). */
#define B5945_OLED_H_BLANK  64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5945_h_blank(void)
{
	return B5945_OLED_H_BLANK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_oled_h_blank_5945 - Deck OLED horizontal blanking total.
 *
 * Always returns 64. Soft compile-time timing stub for H-blank
 * aggregate (not a real EDID dump). No parent wires.
 */
uint32_t
gj_deck_oled_h_blank_5945(void)
{
	(void)NULL;
	return b5945_h_blank();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_oled_h_blank_5945(void)
    __attribute__((alias("gj_deck_oled_h_blank_5945")));
