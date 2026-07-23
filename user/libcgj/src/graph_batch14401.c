/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14401: continuum product deepen root tag
 * (wave 44).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_14401(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_14401  (alias)
 *   __libcgj_batch14401_marker = "libcgj-batch14401"
 *
 * Exclusive continuum CREATE-ONLY (14401-14410: continuum product
 * deepen wave 44, post-14400). Unique gj_continuum_product_deepen_root_14401
 * surface only; no multi-def. Distinct from prior continuum_product_deepen / continuum_smoke_soft /
 * continuum_dyn_soft symbols (1430x/1420x/1410x/…). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14401_marker[] = "libcgj-batch14401";

/* Continuum product deepen root fourCC "CPDN". */
#define B14401_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14401_root(void)
{
	return B14401_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_14401 - continuum product deepen root
 *
 * Always returns 0x4350444Eu. Soft pure-data product tag. Does not call
 * libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_14401(void)
{
	(void)NULL;
	return b14401_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_14401(void)
    __attribute__((alias("gj_continuum_product_deepen_root_14401")));
