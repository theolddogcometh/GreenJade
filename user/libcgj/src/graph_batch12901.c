/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12901: continuum product deepen root tag
 * (wave 29).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_12901(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_12901  (alias)
 *   __libcgj_batch12901_marker = "libcgj-batch12901"
 *
 * Exclusive continuum CREATE-ONLY (12901-12910: continuum product
 * deepen wave 29, post-12900). Unique gj_continuum_product_deepen_root_12901
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_12801,
 * gj_continuum_product_deepen_root_12701, and
 * gj_continuum_product_deepen_root_12601. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12901_marker[] = "libcgj-batch12901";

/* Continuum product deepen root fourCC "CPDN". */
#define B12901_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12901_root(void)
{
	return B12901_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_12901 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 12901-12910 continuum (wave 29). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_12901(void)
{
	(void)NULL;
	return b12901_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_12901(void)
    __attribute__((alias("gj_continuum_product_deepen_root_12901")));
