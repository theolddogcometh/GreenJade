/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2191: product wave identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_wave_id(void);
 *     - Returns the exclusive product-helper wave id for the milestone
 *       2200 continuum (always 2200). Soft compile-time product tag.
 *   uint32_t __gj_product_wave_id  (alias)
 *   __libcgj_batch2191_marker = "libcgj-batch2191"
 *
 * Milestone 2200 exclusive product helpers (2191-2200). Unique
 * gj_product_wave_id surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2191_marker[] = "libcgj-batch2191";

/* Product-helper exclusive wave id for milestone 2200. */
#define B2191_WAVE_ID  2200u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2191_wave_id(void)
{
	return B2191_WAVE_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_wave_id - report the exclusive product-helper wave id.
 *
 * Always returns 2200 (milestone 2200 product continuum). Link-time
 * presence of this symbol tags the product wave. Does not call libc.
 */
uint32_t
gj_product_wave_id(void)
{
	(void)NULL;
	return b2191_wave_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_wave_id(void)
    __attribute__((alias("gj_product_wave_id")));
