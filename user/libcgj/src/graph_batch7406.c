/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7406: continuum product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_code_7406(void);
 *     - Returns the soft continuum product-deepen code tag for this
 *       continuum (always 0x43443734, fourCC "CD74"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_continuum_product_deepen_code_7406  (alias)
 *   __libcgj_batch7406_marker = "libcgj-batch7406"
 *
 * Exclusive continuum CREATE-ONLY (7401-7410: post-7400 continuum
 * product deepen). Unique gj_continuum_product_deepen_code_7406
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_code_7206 ("CD72"),
 * gj_continuum_product_deepen_code_7006 ("CD70"),
 * gj_continuum_product_deepen_code_6806, and
 * gj_continuum_product_deepen_ok_7405. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7406_marker[] = "libcgj-batch7406";

/* Continuum product deepen code fourCC "CD74". */
#define B7406_CPD_CODE  0x43443734u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7406_code(void)
{
	return B7406_CPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_code_7406 - continuum product deepen code.
 *
 * Always returns 0x43443734 ("CD74"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_code_7406(void)
{
	(void)NULL;
	return b7406_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_code_7406(void)
    __attribute__((alias("gj_continuum_product_deepen_code_7406")));
