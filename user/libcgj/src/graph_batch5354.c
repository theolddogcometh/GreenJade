/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5354: Steam Deck native resolution predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_is_native_u(uint32_t w, uint32_t h);
 *     - Return 1 when (w,h) is exactly Deck native 1280x800, else 0.
 *   uint32_t __gj_deck_is_native_u  (alias)
 *   __libcgj_batch5354_marker = "libcgj-batch5354"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique). Unique gj_deck_is_native_u surface only; no multi-def.
 * Distinct from gj_deck_res_is_native_hint (batch1868 int surface),
 * gj_deck_res_w_u / gj_deck_res_h_u (batch5352/5353), and
 * gj_mode_is_hd_u (batch5303). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5354_marker[] = "libcgj-batch5354";

/* Steam Deck LCD native panel size. */
#define B5354_DECK_W  1280u
#define B5354_DECK_H  800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5354_is_native(uint32_t u32W, uint32_t u32H)
{
	return (u32W == B5354_DECK_W && u32H == B5354_DECK_H) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_is_native_u - true when dimensions are Deck native 1280x800.
 *
 * w: frame width in pixels
 * h: frame height in pixels
 *
 * Returns 1 if w==1280 and h==800, else 0. Exact match only; no
 * orientation swap (800x1280 -> 0). Pure data; no mode objects.
 * No parent wires.
 */
uint32_t
gj_deck_is_native_u(uint32_t u32W, uint32_t u32H)
{
	(void)NULL;
	return b5354_is_native(u32W, u32H);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_is_native_u(uint32_t u32W, uint32_t u32H)
    __attribute__((alias("gj_deck_is_native_u")));
