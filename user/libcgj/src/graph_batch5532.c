/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5532: nested refresh rate (Hz) soft pick (_5532).
 *
 * Surface (unique symbols):
 *   uint32_t gj_nested_refresh_hz_5532(uint32_t outer_hz, uint32_t prefer_hz);
 *     - Choose a nested (inner) refresh rate for a gamescope-style stack.
 *       If prefer_hz == 0, return outer_hz (passthrough).
 *       If prefer_hz is an integer multiple of outer_hz (or vice versa
 *       within safe bounds), return prefer_hz; else return outer_hz.
 *       outer_hz == 0 → 0.
 *   uint32_t __gj_nested_refresh_hz_5532  (alias)
 *   __libcgj_batch5532_marker = "libcgj-batch5532"
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

const char __libcgj_batch5532_marker[] = "libcgj-batch5532";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5532_nested_hz(uint32_t u32Outer, uint32_t u32Prefer)
{
	if (u32Outer == 0u) {
		return 0u;
	}
	if (u32Prefer == 0u) {
		return u32Outer;
	}
	/* Prefer must be integer multiple of outer, or outer of prefer. */
	if ((u32Prefer % u32Outer) == 0u) {
		return u32Prefer;
	}
	if ((u32Outer % u32Prefer) == 0u) {
		return u32Prefer;
	}
	return u32Outer;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nested_refresh_hz_5532 - soft nested refresh Hz for compositor stack.
 *
 * outer_hz:  outer / panel refresh (0 → 0)
 * prefer_hz: preferred nested rate (0 → outer passthrough)
 *
 * Returns prefer_hz when it is an integer multiple relationship with
 * outer_hz; otherwise outer_hz. Pure integer; no timer hardware.
 */
uint32_t
gj_nested_refresh_hz_5532(uint32_t u32OuterHz, uint32_t u32PreferHz)
{
	(void)NULL;
	return b5532_nested_hz(u32OuterHz, u32PreferHz);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nested_refresh_hz_5532(uint32_t u32OuterHz, uint32_t u32PreferHz)
    __attribute__((alias("gj_nested_refresh_hz_5532")));
