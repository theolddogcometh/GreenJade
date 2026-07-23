/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6606: continuum product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_code_6606(void);
 *     - Returns the soft continuum product-deepen code tag for this
 *       continuum (always 0x43443631, fourCC "CD61"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_continuum_product_deepen_code_6606  (alias)
 *   __libcgj_batch6606_marker = "libcgj-batch6606"
 *
 * Exclusive continuum CREATE-ONLY (6601-6610: post-6600 continuum
 * product deepen). Unique gj_continuum_product_deepen_code_6606
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_code_6306,
 * gj_product_install_deepen_code_6106, and
 * gj_continuum_product_deepen_ok_6605. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6606_marker[] = "libcgj-batch6606";

/* Continuum product deepen code fourCC "CD61". */
#define B6606_CPD_CODE  0x43443631u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6606_code(void)
{
	return B6606_CPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_code_6606 - continuum product deepen code.
 *
 * Always returns 0x43443631 ("CD61"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_code_6606(void)
{
	(void)NULL;
	return b6606_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_code_6606(void)
    __attribute__((alias("gj_continuum_product_deepen_code_6606")));
