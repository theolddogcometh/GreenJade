/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3349: product wave identity for milestone 3350.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_wave_3350(void);
 *     - Returns the exclusive product-helper wave id for the milestone
 *       3350 continuum (always 3350). Soft compile-time product tag.
 *   uint32_t __gj_product_wave_3350  (alias)
 *   __libcgj_batch3349_marker = "libcgj-batch3349"
 *
 * Milestone 3350 exclusive continuum CREATE-ONLY (3341-3350). Distinct
 * from gj_product_wave_3250 (batch3241), gj_product_wave_3150
 * (batch3149), and gj_product_wave_id (batch2191) — unique
 * gj_product_wave_3350 surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3349_marker[] = "libcgj-batch3349";

/* Product-helper exclusive wave id for milestone 3350. */
#define B3349_WAVE_ID  3350u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3349_wave_id(void)
{
	return B3349_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_wave_3350 - report the exclusive milestone-3350 product wave.
 *
 * Always returns 3350 (MILESTONE 3350 product continuum). Link-time
 * presence of this symbol tags the product wave. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_product_wave_3350(void)
{
	(void)NULL;
	return b3349_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_wave_3350(void)
    __attribute__((alias("gj_product_wave_3350")));
