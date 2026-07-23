/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5944: Deck OLED panel vertical active lines.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_oled_v_active_5944(void);
 *     - Returns 800 (Deck OLED native vertical active lines).
 *   uint32_t __gj_deck_oled_v_active_5944  (alias)
 *   __libcgj_batch5944_marker = "libcgj-batch5944"
 *
 * Exclusive continuum CREATE-ONLY (5941-5950: Deck OLED panel timing
 * stubs —
 * deck_oled_refresh_max_hz_5941, deck_oled_refresh_min_hz_5942,
 * deck_oled_h_active_5943, deck_oled_v_active_5944,
 * deck_oled_h_blank_5945, deck_oled_v_blank_5946,
 * deck_oled_frame_us_90_5947, deck_oled_pixel_clk_khz_5948,
 * deck_oled_timing_ready_5949, batch_id_5950). Unique surface only;
 * no multi-def. Distinct from gj_deck_res_h_u (batch5353). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5944_marker[] = "libcgj-batch5944";

/* Deck OLED native vertical active lines. */
#define B5944_OLED_V_ACTIVE  800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5944_v_active(void)
{
	return B5944_OLED_V_ACTIVE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_oled_v_active_5944 - Deck OLED vertical active timing.
 *
 * Always returns 800. Soft compile-time panel timing stub; does not
 * probe EDID. No parent wires.
 */
uint32_t
gj_deck_oled_v_active_5944(void)
{
	(void)NULL;
	return b5944_v_active();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_oled_v_active_5944(void)
    __attribute__((alias("gj_deck_oled_v_active_5944")));
