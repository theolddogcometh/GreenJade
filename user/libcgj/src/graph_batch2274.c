/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2274: Steam Deck resolution scale percent
 * relative to native LCD panel (unsigned).
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_res_scale_u(unsigned w, unsigned h);
 *     - Return the integer percent scale of (w, h) versus the Steam Deck
 *       LCD native 1280x800, using the lesser of the two axis percents:
 *         min((w * 100) / 1280, (h * 100) / 800).
 *       Zero width or height -> 0. Soft display-mode helper only.
 *   uint32_t __gj_deck_res_scale_u  (alias)
 *   __libcgj_batch2274_marker = "libcgj-batch2274"
 *
 * Post-2270 product exclusive wave (2271-2280). Distinct from
 * gj_deck_res_is_native_hint (batch1868) - unique gj_deck_res_scale_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2274_marker[] = "libcgj-batch2274";

/* Steam Deck LCD native panel size. */
#define B2274_DECK_W  1280u
#define B2274_DECK_H  800u

/* ---- freestanding helpers ---------------------------------------------- */

/* Percent of native along the tighter axis; 0 if either dim is zero. */
static uint32_t
b2274_scale(unsigned uW, unsigned uH)
{
	uint32_t uPctW;
	uint32_t uPctH;

	if (uW == 0u || uH == 0u) {
		return 0u;
	}
	uPctW = ((uint32_t)uW * 100u) / B2274_DECK_W;
	uPctH = ((uint32_t)uH * 100u) / B2274_DECK_H;
	if (uPctW < uPctH) {
		return uPctW;
	}
	return uPctH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_res_scale_u - percent scale of (w, h) vs Deck native 1280x800.
 *
 * w, h: pixel width and height
 *
 * Returns min((w*100)/1280, (h*100)/800), or 0 if either dimension is 0.
 * Native 1280x800 -> 100. Half linear -> 50. Does not call libc.
 */
uint32_t
gj_deck_res_scale_u(unsigned uW, unsigned uH)
{
	(void)NULL;
	return b2274_scale(uW, uH);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_res_scale_u(unsigned uW, unsigned uH)
    __attribute__((alias("gj_deck_res_scale_u")));
