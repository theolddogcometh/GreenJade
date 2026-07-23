/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3049: milestone 3050 product export hint.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_hint_3050(void);
 *     - Returns the product dynamic-export / readiness hint tag for
 *       the milestone 3050 exclusive wave (always 3050). Soft
 *       compile-time product tag.
 *   uint32_t __gj_product_hint_3050  (alias)
 *   __libcgj_batch3049_marker = "libcgj-batch3049"
 *
 * Milestone 3050 exclusive CREATE-ONLY (3041-3050). Unique
 * gj_product_hint_3050 surface only; no multi-def. Distinct from
 * gj_export_hint_3000 (batch2992), gj_export_hint_2700 (batch2692),
 * gj_export_hint_2600 (batch2592), and gj_batch_id_3050 (batch3050).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3049_marker[] = "libcgj-batch3049";

/* Product export / readiness hint tag for wave 3050. */
#define B3049_PRODUCT_HINT  3050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3049_product_hint(void)
{
	return B3049_PRODUCT_HINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_hint_3050 - report the milestone-3050 product hint tag.
 *
 * Always returns 3050. Link-time presence of this symbol tags the wave.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_product_hint_3050(void)
{
	(void)NULL;
	return b3049_product_hint();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_hint_3050(void)
    __attribute__((alias("gj_product_hint_3050")));
