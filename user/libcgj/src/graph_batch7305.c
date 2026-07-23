/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7305: bar3 product deepen phase gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_product_deepen_ok_7305(uint32_t phase_id);
 *     - Return 1 if phase_id is the verify-seal/ready slot (6 or 7)
 *       for the bar3 product-deepen phase table; else 0. Soft
 *       pure-data gate; does not execute install or product action.
 *   uint32_t __gj_bar3_product_deepen_ok_7305  (alias)
 *   __libcgj_batch7305_marker = "libcgj-batch7305"
 *
 * Exclusive continuum CREATE-ONLY (7301-7310: post-7300 bar3 product
 * deepen). Unique gj_bar3_product_deepen_ok_7305 surface only; no
 * multi-def. Distinct from gj_bar3_product_deepen_ok_7105,
 * gj_bar3_product_deepen_ok_6905, and
 * gj_bar3_product_deepen_ok_6705. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7305_marker[] = "libcgj-batch7305";

/* Verify seal = 6, ready = 7 (see batch7302 phase table). */
#define B7305_PHASE_VERIFY  6u
#define B7305_PHASE_READY   7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7305_ok(uint32_t u32Phase)
{
	if (u32Phase == B7305_PHASE_VERIFY ||
	    u32Phase == B7305_PHASE_READY) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_product_deepen_ok_7305 - true on verify/ready BPD phases.
 *
 * phase_id: bar3 product-deepen phase index (pairs with batch7302)
 *
 * Returns 1 when phase is verify seal (6) or ready (7), else 0.
 * Soft pure-data gate. No parent wires.
 */
uint32_t
gj_bar3_product_deepen_ok_7305(uint32_t phase_id)
{
	(void)NULL;
	return b7305_ok(phase_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_product_deepen_ok_7305(uint32_t phase_id)
    __attribute__((alias("gj_bar3_product_deepen_ok_7305")));
