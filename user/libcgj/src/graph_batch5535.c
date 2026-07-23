/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5535: Steam Deck UI chrome height soft (_5535).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_ui_chrome_5535(uint32_t mode);
 *     - Return a soft Deck-style UI chrome height in pixels for mode:
 *         0 → 0 (hidden / game-only)
 *         1 → 40 (compact top bar)
 *         2 → 64 (standard quick-access chrome)
 *         3 → 96 (expanded overlay chrome)
 *       Any other mode → 64 (default standard).
 *   uint32_t __gj_deck_ui_chrome_5535  (alias)
 *   __libcgj_batch5535_marker = "libcgj-batch5535"
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

const char __libcgj_batch5535_marker[] = "libcgj-batch5535";

#define B5535_CHROME_HIDDEN   0u
#define B5535_CHROME_COMPACT  40u
#define B5535_CHROME_STD      64u
#define B5535_CHROME_EXPAND   96u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5535_chrome(uint32_t u32Mode)
{
	if (u32Mode == 0u) {
		return B5535_CHROME_HIDDEN;
	}
	if (u32Mode == 1u) {
		return B5535_CHROME_COMPACT;
	}
	if (u32Mode == 2u) {
		return B5535_CHROME_STD;
	}
	if (u32Mode == 3u) {
		return B5535_CHROME_EXPAND;
	}
	return B5535_CHROME_STD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_ui_chrome_5535 - soft Deck UI chrome height in pixels.
 *
 * mode: 0 hidden, 1 compact, 2 standard, 3 expanded; else standard.
 *
 * Returns chrome height in pixels for layout math. Pure table stub; no
 * UI toolkit. No parent wires.
 */
uint32_t
gj_deck_ui_chrome_5535(uint32_t u32Mode)
{
	(void)NULL;
	return b5535_chrome(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_ui_chrome_5535(uint32_t u32Mode)
    __attribute__((alias("gj_deck_ui_chrome_5535")));
