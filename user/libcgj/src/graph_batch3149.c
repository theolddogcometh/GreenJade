/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3149: product wave identity for milestone 3150.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_wave_3150(void);
 *     - Returns the exclusive product-helper wave id for the milestone
 *       3150 continuum (always 3150). Soft compile-time product tag.
 *   uint32_t __gj_product_wave_3150  (alias)
 *   __libcgj_batch3149_marker = "libcgj-batch3149"
 *
 * Milestone 3150 exclusive continuum CREATE-ONLY (3141-3150). Distinct
 * from gj_product_wave_id (batch2191 wave 2200) and prior wave ids —
 * unique gj_product_wave_3150 surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3149_marker[] = "libcgj-batch3149";

/* Product-helper exclusive wave id for milestone 3150. */
#define B3149_WAVE_ID  3150u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3149_wave_id(void)
{
	return B3149_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_wave_3150 - report the exclusive milestone-3150 product wave.
 *
 * Always returns 3150 (MILESTONE 3150 product continuum). Link-time
 * presence of this symbol tags the product wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_product_wave_3150(void)
{
	(void)NULL;
	return b3149_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_wave_3150(void)
    __attribute__((alias("gj_product_wave_3150")));
