/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5947: Deck OLED frame period at 90 Hz.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_oled_frame_us_90_5947(void);
 *     - Returns 11111 (approximate frame period in microseconds at
 *       90 Hz: floor(1e6 / 90)).
 *   uint32_t __gj_deck_oled_frame_us_90_5947  (alias)
 *   __libcgj_batch5947_marker = "libcgj-batch5947"
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

const char __libcgj_batch5947_marker[] = "libcgj-batch5947";

/* Approximate frame period at 90 Hz (us): floor(1000000 / 90). */
#define B5947_OLED_FRAME_US_90  11111u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5947_frame_us_90(void)
{
	return B5947_OLED_FRAME_US_90;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_oled_frame_us_90_5947 - Deck OLED 90 Hz frame period (us).
 *
 * Always returns 11111. Soft compile-time timing stub for vsync
 * period math; does not sample a timer. No parent wires.
 */
uint32_t
gj_deck_oled_frame_us_90_5947(void)
{
	(void)NULL;
	return b5947_frame_us_90();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_oled_frame_us_90_5947(void)
    __attribute__((alias("gj_deck_oled_frame_us_90_5947")));
