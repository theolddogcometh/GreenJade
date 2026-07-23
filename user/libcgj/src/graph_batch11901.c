/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11901: continuum product deepen root tag
 * (wave 19).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_11901(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_11901  (alias)
 *   __libcgj_batch11901_marker = "libcgj-batch11901"
 *
 * Exclusive continuum CREATE-ONLY (11901-11910: continuum product
 * deepen wave 19). Unique gj_continuum_product_deepen_root_11901
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_11801,
 * gj_continuum_product_deepen_root_11701, and
 * gj_continuum_product_deepen_root_11501. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11901_marker[] = "libcgj-batch11901";

/* Continuum product deepen root fourCC "CPDN". */
#define B11901_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11901_root(void)
{
	return B11901_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_11901 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 11901-11910 continuum (wave 19). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_11901(void)
{
	(void)NULL;
	return b11901_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_11901(void)
    __attribute__((alias("gj_continuum_product_deepen_root_11901")));
