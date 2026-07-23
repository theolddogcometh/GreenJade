/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13901: continuum product deepen root tag
 * (wave 39).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_13901(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_13901  (alias)
 *   __libcgj_batch13901_marker = "libcgj-batch13901"
 *
 * Exclusive continuum CREATE-ONLY (13901-13910: continuum product
 * deepen wave 39, post-13900). Unique gj_continuum_product_deepen_root_13901
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

const char __libcgj_batch13901_marker[] = "libcgj-batch13901";

/* Continuum product deepen root fourCC "CPDN". */
#define B13901_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13901_root(void)
{
	return B13901_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_13901 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 13901-13910 continuum (wave 39). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_13901(void)
{
	(void)NULL;
	return b13901_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_13901(void)
    __attribute__((alias("gj_continuum_product_deepen_root_13901")));
