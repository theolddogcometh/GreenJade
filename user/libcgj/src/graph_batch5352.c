/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5352: Steam Deck native panel width.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_res_w_u(void);
 *     - Returns 1280 (Steam Deck LCD native width in pixels). Soft
 *       compile-time product-path constant.
 *   uint32_t __gj_deck_res_w_u  (alias)
 *   __libcgj_batch5352_marker = "libcgj-batch5352"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique). Unique gj_deck_res_w_u surface only; no multi-def.
 * Distinct from gj_deck_res_is_native_hint (batch1868) and
 * gj_deck_res_scale_u (batch2274). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5352_marker[] = "libcgj-batch5352";

/* Steam Deck LCD native width. */
#define B5352_DECK_W  1280u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5352_deck_w(void)
{
	return B5352_DECK_W;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_res_w_u - report Steam Deck native panel width in pixels.
 *
 * Always returns 1280. Soft pure-data product-path constant; does not
 * probe display hardware. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_res_w_u(void)
{
	(void)NULL;
	return b5352_deck_w();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_res_w_u(void)
    __attribute__((alias("gj_deck_res_w_u")));
