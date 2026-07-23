/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2597: product bar3 readiness mask constant.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_bar3_mask(void);
 *     - Returns the product bar3 readiness mask (always 0x7: low three
 *       bits Steam/Deck/Top50). Soft compile-time product tag for the
 *       third product-bar wave.
 *   uint32_t __gj_product_bar3_mask  (alias)
 *   __libcgj_batch2597_marker = "libcgj-batch2597"
 *
 * Milestone 2600 exclusive product helpers (2591-2600). Unique
 * gj_product_bar3_mask surface only; no multi-def. Distinct from
 * gj_product_bar3_bits (batch2398 extract), gj_bar3_steam_bit
 * (batch2498), gj_bar3_deck_bit (batch2499), and gj_bar3_ready_hint
 * (batch2299).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2597_marker[] = "libcgj-batch2597";

/* Three product bar3 readiness bits (bits 0..2). */
#define B2597_BAR3_MASK  0x7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2597_bar3_mask(void)
{
	return B2597_BAR3_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_bar3_mask - report the product bar3 readiness mask.
 *
 * Always returns 0x7 (low three bar3 slots). Link-time presence of
 * this symbol tags the bar3 mask constant. Pair with
 * gj_product_bar3_bits for extract and gj_bar3_ready_hint for
 * completeness. Does not call libc.
 */
uint32_t
gj_product_bar3_mask(void)
{
	(void)NULL;
	return b2597_bar3_mask();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_bar3_mask(void)
    __attribute__((alias("gj_product_bar3_mask")));
