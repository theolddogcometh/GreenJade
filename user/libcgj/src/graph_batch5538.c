/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5538: FSR sharpness to Q8 fixed-point (_5538).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fsr_sharp_q8_5538(uint32_t level);
 *     - Map integer FSR sharpness level [0, 10] to Q8 fixed-point in
 *       [0, 256], i.e. round(level * 256 / 10). level is first clamped
 *       to [0, 10] (same domain as gj_fsr_sharpness_5533).
 *   uint32_t __gj_fsr_sharp_q8_5538  (alias)
 *   __libcgj_batch5538_marker = "libcgj-batch5538"
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

const char __libcgj_batch5538_marker[] = "libcgj-batch5538";

#define B5538_SHARP_MAX  10u
#define B5538_Q8_ONE     256u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map level∈[0,10] → Q8∈[0,256] via (level * 256 + 5) / 10 for round.
 * level==10 yields exactly 256.
 */
static uint32_t
b5538_to_q8(uint32_t u32Level)
{
	if (u32Level > B5538_SHARP_MAX) {
		u32Level = B5538_SHARP_MAX;
	}
	return (u32Level * B5538_Q8_ONE + 5u) / B5538_SHARP_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fsr_sharp_q8_5538 - FSR sharpness level to Q8 fixed-point weight.
 *
 * level: integer sharpness (clamped to [0, 10])
 *
 * Returns rounded Q8 in [0, 256]. Pure integer; no floats, no GPU.
 * Complements gj_fsr_sharpness_5533. No parent wires.
 */
uint32_t
gj_fsr_sharp_q8_5538(uint32_t u32Level)
{
	(void)NULL;
	return b5538_to_q8(u32Level);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fsr_sharp_q8_5538(uint32_t u32Level)
    __attribute__((alias("gj_fsr_sharp_q8_5538")));
