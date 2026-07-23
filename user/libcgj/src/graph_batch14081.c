/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14081: continuum product deepen root tag
 * (post-14080 deepen + soft smoke/dyn).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_14081(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_14081  (alias)
 *   __libcgj_batch14081_marker = "libcgj-batch14081"
 *
 * Exclusive continuum CREATE-ONLY (14081-14090: continuum product
 * deepen + soft smoke/dyn, post-14080). Unique
 * gj_continuum_product_deepen_root_14081 surface only; no multi-def.
 * Distinct from gj_continuum_product_deepen_root_14001 (wave 40),
 * gj_continuum_product_deepen_root_13901, and
 * gj_continuum_product_deepen_root_13801. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14081_marker[] = "libcgj-batch14081";

/* Continuum product deepen root fourCC "CPDN". */
#define B14081_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14081_root(void)
{
	return B14081_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_14081 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 14081-14090 continuum (post-14080 deepen). Does not probe paths or
 * call libc. No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_14081(void)
{
	(void)NULL;
	return b14081_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_14081(void)
    __attribute__((alias("gj_continuum_product_deepen_root_14081")));
