/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11701: continuum product deepen root tag
 * (wave 17).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_11701(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_11701  (alias)
 *   __libcgj_batch11701_marker = "libcgj-batch11701"
 *
 * Exclusive continuum CREATE-ONLY (11701-11710: continuum product
 * deepen wave 17). Unique gj_continuum_product_deepen_root_11701
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_11501,
 * gj_continuum_product_deepen_root_11401, and
 * gj_continuum_product_deepen_root_11301. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11701_marker[] = "libcgj-batch11701";

/* Continuum product deepen root fourCC "CPDN". */
#define B11701_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11701_root(void)
{
	return B11701_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_11701 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 11701-11710 continuum (wave 17). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_11701(void)
{
	(void)NULL;
	return b11701_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_11701(void)
    __attribute__((alias("gj_continuum_product_deepen_root_11701")));
