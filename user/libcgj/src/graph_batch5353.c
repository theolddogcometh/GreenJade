/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5353: Steam Deck native panel height.
 *
 * Surface (unique symbols):
 *   uint32_t gj_deck_res_h_u(void);
 *     - Returns 800 (Steam Deck LCD native height in pixels). Soft
 *       compile-time product-path constant.
 *   uint32_t __gj_deck_res_h_u  (alias)
 *   __libcgj_batch5353_marker = "libcgj-batch5353"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique). Unique gj_deck_res_h_u surface only; no multi-def.
 * Distinct from gj_deck_res_w_u (batch5352), gj_deck_res_is_native_hint
 * (batch1868), and gj_deck_res_scale_u (batch2274). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5353_marker[] = "libcgj-batch5353";

/* Steam Deck LCD native height. */
#define B5353_DECK_H  800u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5353_deck_h(void)
{
	return B5353_DECK_H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_res_h_u - report Steam Deck native panel height in pixels.
 *
 * Always returns 800. Soft pure-data product-path constant; does not
 * probe display hardware. Does not call libc. No parent wires.
 */
uint32_t
gj_deck_res_h_u(void)
{
	(void)NULL;
	return b5353_deck_h();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_deck_res_h_u(void)
    __attribute__((alias("gj_deck_res_h_u")));
