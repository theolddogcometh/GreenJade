/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12301: continuum product deepen root tag
 * (wave 23).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_12301(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_12301  (alias)
 *   __libcgj_batch12301_marker = "libcgj-batch12301"
 *
 * Exclusive continuum CREATE-ONLY (12301-12310: continuum product
 * deepen wave 23, post-12300). Unique gj_continuum_product_deepen_root_12301
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_12201,
 * gj_continuum_product_deepen_root_12101, and
 * gj_continuum_product_deepen_root_12001. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12301_marker[] = "libcgj-batch12301";

/* Continuum product deepen root fourCC "CPDN". */
#define B12301_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12301_root(void)
{
	return B12301_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_12301 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 12301-12310 continuum (wave 23). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_12301(void)
{
	(void)NULL;
	return b12301_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_12301(void)
    __attribute__((alias("gj_continuum_product_deepen_root_12301")));
