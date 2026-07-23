/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11401: continuum product deepen root tag
 * (wave 14).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_11401(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_11401  (alias)
 *   __libcgj_batch11401_marker = "libcgj-batch11401"
 *
 * Exclusive continuum CREATE-ONLY (11401-11410: continuum product
 * deepen wave 14). Unique gj_continuum_product_deepen_root_11401
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_11301,
 * gj_continuum_product_deepen_root_11201, and
 * gj_continuum_product_deepen_root_11101. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11401_marker[] = "libcgj-batch11401";

/* Continuum product deepen root fourCC "CPDN". */
#define B11401_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11401_root(void)
{
	return B11401_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_11401 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 11401-11410 continuum (wave 14). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_11401(void)
{
	(void)NULL;
	return b11401_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_11401(void)
    __attribute__((alias("gj_continuum_product_deepen_root_11401")));
