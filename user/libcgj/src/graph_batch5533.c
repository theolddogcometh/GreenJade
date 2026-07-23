/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5533: FSR sharpness clamp (0..10 scale) (_5533).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsr_sharpness_5533(uint32_t level);
 *     - Clamp an FSR-style sharpness control to the closed integer
 *       range [0, 10]. Values above 10 saturate to 10.
 *   uint32_t __gj_fsr_sharpness_5533  (alias)
 *   __libcgj_batch5533_marker = "libcgj-batch5533"
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

const char __libcgj_batch5533_marker[] = "libcgj-batch5533";

/* Soft FSR sharpness UI max (gamescope-style 0..10 slider). */
#define B5533_SHARP_MAX  10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5533_clamp(uint32_t u32Level)
{
	if (u32Level > B5533_SHARP_MAX) {
		return B5533_SHARP_MAX;
	}
	return u32Level;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsr_sharpness_5533 - clamp FSR sharpness to [0, 10].
 *
 * level: requested sharpness (integer UI units)
 *
 * Returns level if in range, else 10. Pure integer stub; no GPU shader
 * dispatch. No parent wires.
 */
uint32_t
gj_fsr_sharpness_5533(uint32_t u32Level)
{
	(void)NULL;
	return b5533_clamp(u32Level);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsr_sharpness_5533(uint32_t u32Level)
    __attribute__((alias("gj_fsr_sharpness_5533")));
