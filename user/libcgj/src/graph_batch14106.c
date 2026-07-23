/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14106: continuum product deepen code tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_code_14106(void);
 *     - Returns the soft continuum product-deepen code tag for this
 *       continuum (always 0x43443134, fourCC "CD14"). Not a live
 *       image-write or install action.
 *   uint32_t __gj_continuum_product_deepen_code_14106  (alias)
 *   __libcgj_batch14106_marker = "libcgj-batch14106"
 *
 * Exclusive continuum CREATE-ONLY (14101-14110: continuum product
 * deepen wave 2). Unique gj_continuum_product_deepen_code_14106
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_code_7006,
 * gj_continuum_product_deepen_code_6806,
 * gj_continuum_product_deepen_code_6606,
 * gj_continuum_product_deepen_code_6306, and
 * gj_continuum_product_deepen_ok_14105. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14106_marker[] = "libcgj-batch14106";

/* Continuum product deepen code fourCC "CD14" (wave-2 14xxx family). */
#define B14106_CPD_CODE  0x43443134u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14106_code(void)
{
	return B14106_CPD_CODE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_code_14106 - continuum product deepen code.
 *
 * Always returns 0x43443134 ("CD14"). Soft pure-data code tag.
 * Does not write images or call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_code_14106(void)
{
	(void)NULL;
	return b14106_code();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_code_14106(void)
    __attribute__((alias("gj_continuum_product_deepen_code_14106")));
