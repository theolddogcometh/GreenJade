/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5950: batch identity for Deck OLED timing wave.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5950(void);
 *     - Returns the compile-time graph batch number for this TU (5950).
 *   uint32_t __gj_batch_id_5950  (alias)
 *   __libcgj_batch5950_marker = "libcgj-batch5950"
 *
 * Exclusive continuum CREATE-ONLY (5941-5950: Deck OLED panel timing
 * stubs —
 * deck_oled_refresh_max_hz_5941, deck_oled_refresh_min_hz_5942,
 * deck_oled_h_active_5943, deck_oled_v_active_5944,
 * deck_oled_h_blank_5945, deck_oled_v_blank_5946,
 * deck_oled_frame_us_90_5947, deck_oled_pixel_clk_khz_5948,
 * deck_oled_timing_ready_5949, batch_id_5950). Unique gj_batch_id_5950
 * surface only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5950_marker[] = "libcgj-batch5950";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5950_id(void)
{
	return 5950u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5950 - report this TU's graph batch number.
 *
 * Always returns 5950.
 */
uint32_t
gj_batch_id_5950(void)
{
	(void)NULL;
	return b5950_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5950(void)
    __attribute__((alias("gj_batch_id_5950")));
