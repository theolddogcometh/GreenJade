/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12001: continuum product deepen root tag
 * (wave 20).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_12001(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_12001  (alias)
 *   __libcgj_batch12001_marker = "libcgj-batch12001"
 *
 * Exclusive continuum CREATE-ONLY (12001-12010: continuum product
 * deepen wave 20, post-12000). Unique gj_continuum_product_deepen_root_12001
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_11901,
 * gj_continuum_product_deepen_root_11801, and
 * gj_continuum_product_deepen_root_11701. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12001_marker[] = "libcgj-batch12001";

/* Continuum product deepen root fourCC "CPDN". */
#define B12001_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12001_root(void)
{
	return B12001_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_12001 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 12001-12010 continuum (wave 20). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_12001(void)
{
	(void)NULL;
	return b12001_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_12001(void)
    __attribute__((alias("gj_continuum_product_deepen_root_12001")));
