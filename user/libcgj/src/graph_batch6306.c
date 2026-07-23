/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6306: continuum product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_code_6306(void);
 *     - Returns the soft continuum product-deepen code tag for this
 *       continuum (always 0x43443331, fourCC "CD31"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_continuum_product_deepen_code_6306  (alias)
 *   __libcgj_batch6306_marker = "libcgj-batch6306"
 *
 * Exclusive continuum CREATE-ONLY (6301-6310: post-6300 continuum
 * product deepen). Unique gj_continuum_product_deepen_code_6306
 * surface only; no multi-def. Distinct from
 * gj_product_install_deepen_code_6106 and
 * gj_continuum_product_deepen_ok_6305. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6306_marker[] = "libcgj-batch6306";

/* Continuum product deepen code fourCC "CD31". */
#define B6306_CPD_CODE  0x43443331u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6306_code(void)
{
	return B6306_CPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_code_6306 - continuum product deepen code.
 *
 * Always returns 0x43443331 ("CD31"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_code_6306(void)
{
	(void)NULL;
	return b6306_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_code_6306(void)
    __attribute__((alias("gj_continuum_product_deepen_code_6306")));
