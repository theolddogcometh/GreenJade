/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10501: continuum product deepen root tag
 * (wave 5).
 *
 * Surface (unique symbols):
 *   uint32_t gj_continuum_product_deepen_root_10501(void);
 *     - Returns the soft continuum product-deepen root layout tag for
 *       this continuum (always 0x4350444E, fourCC "CPDN"). Not a
 *       filesystem path probe.
 *   uint32_t __gj_continuum_product_deepen_root_10501  (alias)
 *   __libcgj_batch10501_marker = "libcgj-batch10501"
 *
 * Exclusive continuum CREATE-ONLY (10501-10510: continuum product
 * deepen wave 5). Unique gj_continuum_product_deepen_root_10501
 * surface only; no multi-def. Distinct from
 * gj_continuum_product_deepen_root_10301,
 * gj_continuum_product_deepen_root_10201, and
 * gj_continuum_product_deepen_root_10101. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10501_marker[] = "libcgj-batch10501";

/* Continuum product deepen root fourCC "CPDN". */
#define B10501_CPD_ROOT  0x4350444Eu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10501_root(void)
{
	return B10501_CPD_ROOT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_continuum_product_deepen_root_10501 - continuum product deepen root.
 *
 * Always returns 0x4350444E ("CPDN"). Soft pure-data layout tag for the
 * 10501-10510 continuum (wave 5). Does not probe paths or call libc.
 * No parent wires.
 */
uint32_t
gj_continuum_product_deepen_root_10501(void)
{
	(void)NULL;
	return b10501_root();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_continuum_product_deepen_root_10501(void)
    __attribute__((alias("gj_continuum_product_deepen_root_10501")));
