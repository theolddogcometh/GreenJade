/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3241: product wave identity for milestone 3250.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_wave_3250(void);
 *     - Returns the exclusive product-helper wave id for the milestone
 *       3250 continuum (always 3250). Soft compile-time product tag.
 *   uint32_t __gj_product_wave_3250  (alias)
 *   __libcgj_batch3241_marker = "libcgj-batch3241"
 *
 * Milestone 3250 exclusive continuum CREATE-ONLY (3241-3250). Distinct
 * from gj_product_wave_3150 (batch3149), gj_product_wave_id (batch2191
 * wave 2200), and gj_wave_id_3200 (batch3192) — unique
 * gj_product_wave_3250 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3241_marker[] = "libcgj-batch3241";

/* Product-helper exclusive wave id for milestone 3250. */
#define B3241_WAVE_ID  3250u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3241_wave_id(void)
{
	return B3241_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_wave_3250 - report the exclusive milestone-3250 product wave.
 *
 * Always returns 3250 (MILESTONE 3250 product continuum). Link-time
 * presence of this symbol tags the product wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_product_wave_3250(void)
{
	(void)NULL;
	return b3241_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_wave_3250(void)
    __attribute__((alias("gj_product_wave_3250")));
