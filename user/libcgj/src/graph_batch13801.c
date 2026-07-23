/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13801: continuum product deepen root tag
 * (wave 38).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_13801(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_13801  (alias)
 *   __libcgj_batch13801_marker = "libcgj-batch13801"
 *
 * Exclusive continuum CREATE-ONLY (13801-13810: continuum product
 * deepen wave 38, post-13800). Unique gj_continuum_product_deepen_root_13801
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_13701,
 * gj_continuum_product_deepen_root_13601, and
 * gj_continuum_product_deepen_root_13501. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13801_marker[] = "libcgj-batch13801";

/* Continuum product deepen root fourCC "CPDN". */
#define B13801_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13801_root(void)
{
	return B13801_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_13801 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 13801-13810 continuum (wave 38). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_13801(void)
{
	(void)NULL;
	return b13801_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_13801(void)
    __attribute__((alias("gj_continuum_product_deepen_root_13801")));
