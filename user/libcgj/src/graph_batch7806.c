/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7806: continuum product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_code_7806(void);
 *     - Returns the soft continuum product-deepen code tag for this
 *       continuum (always 0x43443738, fourCC "CD78"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_continuum_product_deepen_code_7806  (alias)
 *   __libcgj_batch7806_marker = "libcgj-batch7806"
 *
 * Exclusive continuum CREATE-ONLY (7801-7810: post-7800 continuum
 * product deepen). Unique gj_continuum_product_deepen_code_7806
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_code_7006,
 * gj_continuum_product_deepen_code_6806,
 * gj_bar3_product_deepen_code_7706, and
 * gj_continuum_product_deepen_ok_7805. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7806_marker[] = "libcgj-batch7806";

/* Continuum product deepen code fourCC "CD78". */
#define B7806_CPD_CODE  0x43443738u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7806_code(void)
{
	return B7806_CPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_code_7806 - continuum product deepen code.
 *
 * Always returns 0x43443738 ("CD78"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_code_7806(void)
{
	(void)NULL;
	return b7806_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_code_7806(void)
    __attribute__((alias("gj_continuum_product_deepen_code_7806")));
