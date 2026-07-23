/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6105: product install deepen phase gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_deepen_ok_6105(uint32_t phase_id);
 *     - Return 1 if phase_id is the verify-seal/ready slot (6 or 7)
 *       for the product install-deepen phase table; else 0. Soft
 *       pure-data gate; does not execute install.
 *   uint32_t __gj_product_install_deepen_ok_6105  (alias)
 *   __libcgj_batch6105_marker = "libcgj-batch6105"
 *
 * Exclusive continuum CREATE-ONLY (6101-6110: post-6100 product
 * deepen install). Unique gj_product_install_deepen_ok_6105 surface
 * only; no multi-def. Distinct from gj_bar3_media_finalize_ok_5805
 * and gj_product_install_deepen_phase_6102. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6105_marker[] = "libcgj-batch6105";

/* Verify seal = 6, ready = 7 (see batch6102 phase table). */
#define B6105_PHASE_VERIFY  6u
#define B6105_PHASE_READY   7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6105_ok(uint32_t u32Phase)
{
	if (u32Phase == B6105_PHASE_VERIFY ||
	    u32Phase == B6105_PHASE_READY) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_deepen_ok_6105 - true on verify/ready deepen phases.
 *
 * phase_id: install-deepen phase index (pairs with batch6102 table)
 *
 * Returns 1 when phase is verify seal (6) or ready (7), else 0.
 * Soft pure-data gate. No parent wires.
 */
uint32_t
gj_product_install_deepen_ok_6105(uint32_t phase_id)
{
	(void)NULL;
	return b6105_ok(phase_id);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_install_deepen_ok_6105(uint32_t phase_id)
    __attribute__((alias("gj_product_install_deepen_ok_6105")));
