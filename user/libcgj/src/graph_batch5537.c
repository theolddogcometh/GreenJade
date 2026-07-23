/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5537: nested refresh period in microseconds (_5537).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_refresh_us_5537(uint32_t hz);
 *     - Convert nested refresh Hz to period microseconds:
 *       floor(1_000_000 / hz). hz == 0 → 0. Saturates naturally for
 *       hz >= 1 (period <= 1_000_000).
 *   uint32_t __gj_nested_refresh_us_5537  (alias)
 *   __libcgj_batch5537_marker = "libcgj-batch5537"
 *
 * Exclusive continuum CREATE-ONLY (5531-5540: gamescope compositor —
 * gscope_comp_stub_5531, nested_refresh_hz_5532, fsr_sharpness_5533,
 * integer_scale_5534, deck_ui_chrome_5535, gscope_layer_z_5536,
 * nested_refresh_us_5537, fsr_sharp_q8_5538, integer_scale_fit_5539,
 * batch_id_5540). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5537_marker[] = "libcgj-batch5537";

#define B5537_US_PER_S  1000000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5537_period_us(uint32_t u32Hz)
{
	if (u32Hz == 0u) {
		return 0u;
	}
	return B5537_US_PER_S / u32Hz;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_refresh_us_5537 - nested refresh period from Hz to us.
 *
 * hz: nested refresh rate in Hertz (0 → 0)
 *
 * Returns floor(1_000_000 / hz) microseconds per frame. Complements
 * gj_nested_refresh_hz_5532. Pure integer; no clocks. No parent wires.
 */
uint32_t
gj_nested_refresh_us_5537(uint32_t u32Hz)
{
	(void)NULL;
	return b5537_period_us(u32Hz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_refresh_us_5537(uint32_t u32Hz)
    __attribute__((alias("gj_nested_refresh_us_5537")));
