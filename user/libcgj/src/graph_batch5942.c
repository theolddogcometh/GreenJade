/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5942: Deck OLED panel min VRR refresh rate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_oled_refresh_min_hz_5942(void);
 *     - Returns 40 (Deck OLED panel minimum VRR floor in Hz).
 *   uint32_t __gj_deck_oled_refresh_min_hz_5942  (alias)
 *   __libcgj_batch5942_marker = "libcgj-batch5942"
 *
 * Exclusive continuum CREATE-ONLY (5941-5950: Deck OLED panel timing
 * stubs —
 * deck_oled_refresh_max_hz_5941, deck_oled_refresh_min_hz_5942,
 * deck_oled_h_active_5943, deck_oled_v_active_5944,
 * deck_oled_h_blank_5945, deck_oled_v_blank_5946,
 * deck_oled_frame_us_90_5947, deck_oled_pixel_clk_khz_5948,
 * deck_oled_timing_ready_5949, batch_id_5950). Unique surface only;
 * no multi-def. Distinct from gj_deck_oled_refresh_max_hz_5941.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5942_marker[] = "libcgj-batch5942";

/* Deck OLED panel minimum VRR floor (Hz). */
#define B5942_OLED_REFRESH_MIN_HZ  40u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5942_refresh_min_hz(void)
{
	return B5942_OLED_REFRESH_MIN_HZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_oled_refresh_min_hz_5942 - Deck OLED min VRR refresh floor.
 *
 * Always returns 40. Soft compile-time timing stub; does not program
 * the panel controller. No parent wires.
 */
uint32_t
gj_deck_oled_refresh_min_hz_5942(void)
{
	(void)NULL;
	return b5942_refresh_min_hz();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_oled_refresh_min_hz_5942(void)
    __attribute__((alias("gj_deck_oled_refresh_min_hz_5942")));
